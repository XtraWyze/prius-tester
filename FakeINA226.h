// ======================================================
// FakeINA226.h
// Simulated INA226 for Prius Battery Tester
// ======================================================

#ifndef FAKEINA226_H
#define FAKEINA226_H

class FakeINA226 {

public:

    FakeINA226();

    void begin();

    void discharge(bool running);

    float readVoltage();

    float readCurrent();

    float readPower();

    float readSOC();

private:

    float voltage;
    float current;
    float soc;

};

#endif