#include <Arduino.h>
#include <LittleFS.h>
#include <WebServer.h>

#include "battery.h"
#include "FakeINA226.h"
#include "logger.h"
#include "web_api.h"
#include "results.h"
#include "test_control.h"
#include "test_summary.h"

extern WebServer server;
extern FakeINA226 sensor;
extern BatteryTest battery;

void handleStatus() {

    String json = "{";

    json += "\"voltage\":" + String(sensor.readVoltage(), 2) + ",";
    json += "\"current\":" + String(sensor.readCurrent(), 2) + ",";
    json += "\"power\":"   + String(sensor.readPower(), 2) + ",";
    json += "\"capacity\":" + String(battery.capacity_mAh, 0) + ",";
    json += "\"energy\":" + String(battery.energy_Wh, 2) + ",";
    json += "\"soc\":" + String(sensor.readSOC(), 0) + ",";
    json += "\"running\":" + String(battery.running ? "true" : "false");

    json += "}";

    server.send(200, "application/json", json);

}

void handleStart() {

    beginTest();

    server.send(200, "text/plain", "STARTED");

}

void handleStop() {

    finalizeTest();

    server.send(200, "text/plain", "STOPPED");

}

void handleReset() {

    resetTest();

    server.send(200, "text/plain", "RESET");

}

void handleDownload() {

    if (!LittleFS.exists("/battery_log.csv")) {
        server.send(404, "text/plain", "No battery log found.");
        return;
    }

    File file = LittleFS.open("/battery_log.csv", "r");

    if (!file) {
        server.send(500, "text/plain", "Unable to open log.");
        return;
    }

    server.sendHeader(
        "Content-Disposition",
        "attachment; filename=battery_log.csv");

    server.streamFile(file, "text/csv");

    file.close();
}

void handleDelete() {

    if (LittleFS.exists("/battery_log.csv")) {
        LittleFS.remove("/battery_log.csv");
        server.send(200, "text/plain", "Battery log deleted.");
    } else {
        server.send(404, "text/plain", "No battery log found.");
    }

}

void handleResults()
{
    server.send(200, "application/json", buildTestSummaryJson());
}

void handleDownloadSummary()
{
    if (results.completed) {
        server.sendHeader(
            "Content-Disposition",
            "attachment; filename=test_summary.json");
        server.send(200, "application/json", buildTestSummaryJson());
        return;
    }

    if (!LittleFS.exists("/test_summary.json")) {
        server.send(404, "text/plain", "No test summary found.");
        return;
    }

    File file = LittleFS.open("/test_summary.json", "r");

    if (!file) {
        server.send(500, "text/plain", "Unable to open summary.");
        return;
    }

    server.sendHeader(
        "Content-Disposition",
        "attachment; filename=test_summary.json");

    server.streamFile(file, "application/json");

    file.close();
}
