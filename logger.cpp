// ======================================================
// logger.cpp
// LittleFS Battery Logger
// ======================================================

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>

#include "logger.h"
#include "battery.h"
#include "FakeINA226.h"

// Global sensor (created in PriusBatteryTester.ino)
extern FakeINA226 sensor;


// ------------------------------------------------------
// Initialize LittleFS
// ------------------------------------------------------

void initLogger() {

    if (!LittleFS.begin(true)) {

        Serial.println("LittleFS Failed");

    } else {

        Serial.println("LittleFS Ready");

    }

}


// ------------------------------------------------------
// Create New Log File
// ------------------------------------------------------

void startLogging() {

    File file = LittleFS.open("/battery_log.csv", "w");

    if (!file) {

        Serial.println("LOG CREATE FAILED");
        return;

    }

    file.println("Time,Voltage,Current,Power,mAh,Wh,SOC");

    file.close();

    battery.testStartTime = millis();

    Serial.println("LOG STARTED");

}


// ------------------------------------------------------
// Append One Line of Data
// ------------------------------------------------------

void logData() {

    File file = LittleFS.open("/battery_log.csv", "a");

    if (!file) {

        Serial.println("LOG FAILED");
        return;

    }

    float voltage = sensor.readVoltage();
    float current = sensor.readCurrent();
    float power   = sensor.readPower();
    float soc     = sensor.readSOC();

    unsigned long elapsed =
        (millis() - battery.testStartTime) / 1000;

    file.print(elapsed);
    file.print(",");

    file.print(voltage, 2);
    file.print(",");

    file.print(current, 2);
    file.print(",");

    file.print(power, 2);
    file.print(",");

    file.print(battery.capacity_mAh, 0);
    file.print(",");

    file.print(battery.energy_Wh, 2);
    file.print(",");

    file.println(soc, 0);

    file.close();

}


// ------------------------------------------------------
// List Files
// ------------------------------------------------------

void listFiles() {

    File root = LittleFS.open("/");

    File file = root.openNextFile();

    while (file) {

        Serial.print("FILE: ");
        Serial.println(file.name());

        Serial.print("SIZE: ");
        Serial.println(file.size());

        file = root.openNextFile();

    }

}