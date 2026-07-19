#include "test_summary.h"

#include <LittleFS.h>
#include <time.h>

#include "results.h"

void setResultsTimestamp()
{
    struct tm timeinfo;

    if (getLocalTime(&timeinfo, 0)) {
        strftime(
            results.timestamp,
            sizeof(results.timestamp),
            "%Y-%m-%d %H:%M:%S",
            &timeinfo);
        return;
    }

    unsigned long secs = millis() / 1000;
    unsigned long h = secs / 3600;
    unsigned long m = (secs % 3600) / 60;
    unsigned long s = secs % 60;

    snprintf(
        results.timestamp,
        sizeof(results.timestamp),
        "Session %02lu:%02lu:%02lu",
        h,
        m,
        s);
}

String buildTestSummaryJson()
{
    String json = "{";

    json += "\"completed\":" +
        String(results.completed ? "true" : "false");
    json += ",\"timestamp\":\"" + String(results.timestamp) + "\"";
    json += ",\"grade\":\"" + String(results.grade) + "\"";
    json += ",\"soh\":" + String(results.soh, 1);
    json += ",\"capacity_mAh\":" +
        String(results.capacity_mAh, 1);
    json += ",\"energy_Wh\":" + String(results.energy_Wh, 2);
    json += ",\"elapsedSeconds\":" +
        String(results.elapsedSeconds);
    json += ",\"startVoltage\":" +
        String(results.startVoltage, 2);
    json += ",\"endVoltage\":" +
        String(results.endVoltage, 2);
    json += ",\"minVoltage\":" +
        String(results.minVoltage, 2);
    json += ",\"maxVoltage\":" +
        String(results.maxVoltage, 2);
    json += ",\"averageVoltage\":" +
        String(results.averageVoltage, 2);
    json += ",\"averageCurrent\":" +
        String(results.averageCurrent, 2);

    json += "}";

    return json;
}

void saveTestSummary()
{
    if (!results.completed)
        return;

    File file = LittleFS.open("/test_summary.json", "w");

    if (!file) {
        Serial.println("SUMMARY SAVE FAILED");
        return;
    }

    file.print(buildTestSummaryJson());
    file.close();

    Serial.println("TEST SUMMARY SAVED");
}
