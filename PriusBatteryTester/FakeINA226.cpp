// ======================================================
// FakeINA226.cpp
// Simulated INA226 for Prius Battery Tester
// ======================================================

#include "FakeINA226.h"
#include "config.h"

FakeINA226::FakeINA226() {

    begin();

}

void FakeINA226::begin() {

    voltage = 8.45;
    current = 1.0;
    soc = 100.0;
    discharged_mAh = 0;
    lastDischargeTime = millis();

}

void FakeINA226::updateVoltageFromSOC() {

    if (soc > 80) {

        voltage = 8.45 - ((100 - soc) * 0.005);

    }
    else if (soc > 20) {

        voltage = 7.60 - ((80 - soc) * 0.006);

    }
    else {

        voltage = 7.20 - ((20 - soc) * 0.060);

        if (voltage < BATTERY_CUTOFF)
            voltage = BATTERY_CUTOFF;

    }

}

void FakeINA226::discharge(bool running) {

    if (!running)
        return;

    unsigned long now = millis();

    float seconds =
        (now - lastDischargeTime) / 1000.0;

    lastDischargeTime = now;

    discharged_mAh +=
        current *
        (seconds / 3600.0) *
        1000.0;

    if (discharged_mAh > NOMINAL_CAPACITY_MAH)
        discharged_mAh = NOMINAL_CAPACITY_MAH;

    soc = 100.0 * (1.0 - (discharged_mAh / NOMINAL_CAPACITY_MAH));

    if (soc < 0)
        soc = 0;

    updateVoltageFromSOC();

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
