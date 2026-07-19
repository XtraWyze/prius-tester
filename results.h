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

    unsigned long elapsedSeconds;

    bool completed;

    char grade;

};

extern TestResults results;