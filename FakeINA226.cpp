// ======================================================
// FakeINA226.cpp
// Simulated INA226 for Prius Battery Tester
// ======================================================

#include "FakeINA226.h"

FakeINA226::FakeINA226() {

    begin();

}

void FakeINA226::begin() {

    voltage = 8.45;
    current = 1.0;
    soc = 100.0;

}

void FakeINA226::discharge(bool running) {

    if (!running)
        return;

    // Simulate battery discharge

    soc -= 0.02;

    if (soc < 0)
        soc = 0;

    // Prius NiMH discharge curve

    if (soc > 80) {

        // Full battery
        voltage = 8.45 - ((100 - soc) * 0.005);

    }
    else if (soc > 20) {

        // Plateau region
        voltage = 7.60 - ((80 - soc) * 0.006);

    }
    else {

        // Rapid voltage collapse
        voltage = 7.20 - ((20 - soc) * 0.060);

        if (voltage < 6.0)
            voltage = 6.0;

    }

}

float FakeINA226::readVoltage() {

    return voltage;

}

float FakeINA226::readCurrent() {

    return current;

}

float FakeINA226::readPower() {

    return voltage * current;

}

float FakeINA226::readSOC() {

    return soc;

}