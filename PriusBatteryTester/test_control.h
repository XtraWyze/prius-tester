#pragma once

void beginTest();

void finalizeTest();

void resetTest();

void updateTestSamples(float voltage, float current);

bool checkCutoff(float voltage);
