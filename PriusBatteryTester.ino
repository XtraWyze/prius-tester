// ======================================================
// PriusBatteryTester.ino
// Main Program
// ======================================================

#include <Arduino.h>

#include "config.h"
#include "battery.h"
#include "display.h"
#include "logger.h"
#include "FakeINA226.h"
#include "portal.h"
#include "results.h"
#include "grading.h"

// ------------------------------------------------------
// Global Sensor
// ------------------------------------------------------

FakeINA226 sensor;


// ------------------------------------------------------
// Function Prototypes
// ------------------------------------------------------

void handleButtons();
void calculateCapacity();


// ======================================================
// Setup
// ======================================================

void setup() {

    Serial.begin(115200);

    initLogger();

    //initDisplay();

    initWiFi();

    pinMode(START_BTN, INPUT_PULLUP);
    pinMode(STOP_BTN, INPUT_PULLUP);
    pinMode(RESET_BTN, INPUT_PULLUP);

    sensor.begin();

    battery.lastCapacityTime = millis();

    //updateDisplay();

}


// ======================================================
// Main Loop
// ======================================================

void loop() {

    updateWiFi();

    handleButtons();

    if (millis() - battery.lastUpdate >= 1000) {

        battery.lastUpdate = millis();

        if (battery.running) {

            sensor.discharge(true);

            calculateCapacity();

            // ===== Update test results =====
            float v = sensor.readVoltage();

            if (v > results.maxVoltage)
                results.maxVoltage = v;

            if (v < results.minVoltage)
                results.minVoltage = v;

            results.capacity_mAh = battery.capacity_mAh;
            results.energy_Wh = battery.energy_Wh;
            // ===============================

            logData();

            if (sensor.readVoltage() <= BATTERY_CUTOFF) {

            battery.running = false;

             results.endVoltage = sensor.readVoltage();

            results.elapsedSeconds =
             (millis() - battery.testStartTime) / 1000;

             results.completed = true;

             results.grade =
             calculateGrade(results.capacity_mAh);

    Serial.println("BATTERY CUTOFF");

}

        }

        //updateDisplay();

    }

}


// ======================================================
// Handle Buttons
// ======================================================

void handleButtons() {

    if (!digitalRead(START_BTN)) {

        battery.running = true;

        startLogging();

        battery.lastCapacityTime = millis();

        Serial.println("TEST STARTED");

        delay(300);

    }


    if (!digitalRead(STOP_BTN)) {

        battery.running = false;

        Serial.println("TEST STOPPED");

        listFiles();

        delay(300);

    }


    if (!digitalRead(RESET_BTN)) {

        battery.running = false;

        battery.capacity_mAh = 0;

        battery.energy_Wh = 0;

        sensor.begin();

        Serial.println("RESET");

        //updateDisplay();

        delay(300);

    }

}


// ======================================================
// Capacity Calculation
// ======================================================

void calculateCapacity() {

    unsigned long now = millis();

    float seconds =
        (now - battery.lastCapacityTime) / 1000.0;

    battery.lastCapacityTime = now;

    float current = sensor.readCurrent();

    float voltage = sensor.readVoltage();

    battery.capacity_mAh +=
        current *
        (seconds / 3600.0) *
        1000.0;

    battery.energy_Wh +=
        (voltage * current) *
        (seconds / 3600.0);

}