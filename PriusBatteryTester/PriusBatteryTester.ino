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
#include "test_control.h"

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

            float v = sensor.readVoltage();
            float c = sensor.readCurrent();

            updateTestSamples(v, c);

            logData();

            if (checkCutoff(v)) {

                finalizeTest();

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

        beginTest();

        Serial.println("TEST STARTED");

        delay(300);

    }


    if (!digitalRead(STOP_BTN)) {

        finalizeTest();

        Serial.println("TEST STOPPED");

        listFiles();

        delay(300);

    }


    if (!digitalRead(RESET_BTN)) {

        resetTest();

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
