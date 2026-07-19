// ======================================================
// display.cpp
// OLED Display Functions
// ======================================================

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "display.h"
#include "battery.h"
#include "results.h"
#include "FakeINA226.h"

// These are defined in PriusBatteryTester.ino
extern FakeINA226 sensor;

// OLED object
Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    -1
);

// ------------------------------------------------------
// Initialize Display
// ------------------------------------------------------

void initDisplay() {

    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {

        Serial.println("OLED Failed");

        while (true);

    }

    display.clearDisplay();
    display.display();

}

// ------------------------------------------------------
// Update Display
// ------------------------------------------------------

void updateDisplay() {

    float voltage = sensor.readVoltage();
    float current = sensor.readCurrent();
    float power   = sensor.readPower();
    float soc     = sensor.readSOC();

    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    display.println("Prius Battery Tester");
    display.println("-------------------");

    display.print("Status: ");

    if (battery.running)
        display.println("RUNNING");
    else
        display.println("STOPPED");

    display.print("V: ");
    display.print(voltage, 2);
    display.print("  A: ");
    display.println(current, 2);

    display.print("P: ");
    display.print(power, 2);
    display.println("W");

    display.print("mAh: ");
    display.println(battery.capacity_mAh, 0);

    display.print("Wh: ");
    display.print(battery.energy_Wh, 2);

    display.print(" SOC:");
    display.print(soc, 0);
    display.println("%");

    if (results.completed) {
        display.print("Grade: ");
        display.print(results.grade);
        display.print("  SOH:");
        display.print(results.soh, 0);
        display.println("%");
    }

    display.display();

}