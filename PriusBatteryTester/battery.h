#pragma once

struct BatteryTest {

    bool running;

    float capacity_mAh;

    float energy_Wh;

    unsigned long lastUpdate;

    unsigned long lastCapacityTime;

    unsigned long testStartTime;

    float voltageSum;

    float currentSum;

    unsigned long sampleCount;

};

extern BatteryTest battery;
