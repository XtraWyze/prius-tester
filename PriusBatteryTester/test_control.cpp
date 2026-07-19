#include <Arduino.h>

#include "test_control.h"
#include "config.h"
#include "battery.h"
#include "results.h"
#include "grading.h"
#include "logger.h"
#include "FakeINA226.h"

extern FakeINA226 sensor;

void beginTest()
{
    battery.running = true;
    battery.testStartTime = millis();
    battery.lastCapacityTime = millis();
    battery.capacity_mAh = 0;
    battery.energy_Wh = 0;
    battery.voltageSum = 0;
    battery.currentSum = 0;
    battery.sampleCount = 0;

    resetResults();

    sensor.begin();

    float v = sensor.readVoltage();

    results.startVoltage = v;
    results.maxVoltage = v;
    results.minVoltage = v;

    startLogging();
}

void finalizeTest()
{
    if (!battery.running && results.completed)
        return;

    battery.running = false;

    results.endVoltage = sensor.readVoltage();
    results.elapsedSeconds =
        (millis() - battery.testStartTime) / 1000;
    results.capacity_mAh = battery.capacity_mAh;
    results.energy_Wh = battery.energy_Wh;

    if (battery.sampleCount > 0) {
        results.averageVoltage =
            battery.voltageSum / battery.sampleCount;
        results.averageCurrent =
            battery.currentSum / battery.sampleCount;
    }

    results.soh = calculateSOH(results.capacity_mAh);
    results.grade = calculateGrade(results.capacity_mAh);
    results.completed = true;
}

void resetTest()
{
    battery.running = false;
    battery.capacity_mAh = 0;
    battery.energy_Wh = 0;
    battery.voltageSum = 0;
    battery.currentSum = 0;
    battery.sampleCount = 0;

    resetResults();
    sensor.begin();
}

void updateTestSamples(float voltage, float current)
{
    if (voltage > results.maxVoltage)
        results.maxVoltage = voltage;

    if (voltage < results.minVoltage)
        results.minVoltage = voltage;

    results.capacity_mAh = battery.capacity_mAh;
    results.energy_Wh = battery.energy_Wh;

    battery.voltageSum += voltage;
    battery.currentSum += current;
    battery.sampleCount++;
}

bool checkCutoff(float voltage)
{
    return voltage <= BATTERY_CUTOFF;
}
