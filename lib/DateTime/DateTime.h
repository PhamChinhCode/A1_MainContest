#pragma once
#include <Myconfig.h>
#include <Arduino.h>
#include <ArduinoJson.h>

class DateTime
{
private:
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t dayOfWeek; // 0 = Sunday, 6 = Saturday
    bool isValid;
    void setFromTm(const struct tm &t);

public:
    DateTime();
    DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour = 0, uint8_t minute = 0, uint8_t second = 0);
    DateTime(uint32_t epoch);
    DateTime(String dateStr, String timeStr); // dateStr: "YYYY-MM-DD HH:MM:SS"
    ~DateTime();
    bool begin(const char *tz = "UTC0", const char *ntpServer = "pool.ntp.org", uint32_t timeoutMs = 5000);
    void setTimezone(const char *tz);
    static DateTime now();
    void update();
    bool valid();
    uint8_t getSecond();
    uint8_t getMinute();
    uint8_t getHour();
    uint8_t getDay();
    uint8_t getMonth();
    uint16_t getYear();
    uint8_t getDayOfWeek();  // 0 = Sunday, 6 = Saturday
    String toString();       // "YYYY-MM-DD HH:MM:SS"
    uint32_t getTimestamp(); // seconds since 1970-01-01 00:00:00
    uint32_t toTimestamp() { return getTimestamp(); }
    String convertToDateTime(uint32_t timestamp);                // convert timestamp to "YYYY-MM-DD HH:MM:SS"
    uint32_t convertToTimestamp(String dateStr, String timeStr); // convert "YYYY-MM-DD HH:MM:SS" to timestamp
    String getDateString();                                      // "YYYY-MM-DD"
    String getTimeString();                                      // "HH:MM:SS"
};