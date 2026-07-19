#include <Arduino.h>
#include <LittleFS.h>
#include <WebServer.h>

#include "battery.h"
#include "FakeINA226.h"
#include "logger.h"
#include "web_api.h"
#include "results.h"
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
// ------------------------------------
// Start Test
// ------------------------------------

void handleStart() {

    battery.running = true;

    battery.testStartTime = millis();

    battery.lastCapacityTime = millis();

    startLogging();

    server.send(200, "text/plain", "STARTED");
    
    results.completed = false;

    results.startVoltage = sensor.readVoltage();

    results.maxVoltage = results.startVoltage;

    results.minVoltage = results.startVoltage;
}

// ------------------------------------
// Stop Test
// ------------------------------------

void handleStop() {

    battery.running = false;

    server.send(200, "text/plain", "STOPPED");

}

// ------------------------------------
// Reset Test
// ------------------------------------

void handleReset() {

    battery.running = false;

    battery.capacity_mAh = 0;

    battery.energy_Wh = 0;

    sensor.begin();

    server.send(200, "text/plain", "RESET");

}

// ------------------------------------
// Download CSV
// ------------------------------------

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

// ------------------------------------
// Delete CSV
// ------------------------------------

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
    String json = "{";

    json += "\"completed\":" + String(results.completed ? "true" : "false");
    json += ",\"grade\":\"" + String(results.grade) + "\"";
    json += ",\"capacity\":" + String(results.capacity_mAh,1);
    json += ",\"energy\":" + String(results.energy_Wh,2);
    json += ",\"startVoltage\":" + String(results.startVoltage,2);
    json += ",\"endVoltage\":" + String(results.endVoltage,2);
    json += ",\"minVoltage\":" + String(results.minVoltage,2);
    json += ",\"maxVoltage\":" + String(results.maxVoltage,2);
    json += ",\"time\":" + String(results.elapsedSeconds);

    json += "}";

    server.send(200,"application/json",json);
}