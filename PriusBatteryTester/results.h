#pragma once

struct TestResults {

    float startVoltage;
    float endVoltage;

    float minVoltage;
    float maxVoltage;

    float averageVoltage;

    float capacity_mAh;
    float energy_Wh;

    float averageCurrent;

    float soh;

    unsigned long elapsedSeconds;

    char timestamp[32];

    bool completed;

    char grade;

};

extern TestResults results;

void resetResults();
