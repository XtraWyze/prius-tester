#include "results.h"

TestResults results = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    {0},
    false,
    '-'
};

void resetResults()
{
    results.startVoltage = 0;
    results.endVoltage = 0;
    results.minVoltage = 0;
    results.maxVoltage = 0;
    results.averageVoltage = 0;
    results.capacity_mAh = 0;
    results.energy_Wh = 0;
    results.averageCurrent = 0;
    results.soh = 0;
    results.elapsedSeconds = 0;
    results.timestamp[0] = '\0';
    results.completed = false;
    results.grade = '-';
}
