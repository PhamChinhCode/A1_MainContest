#pragma once
#include <Myconfig.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

struct DisplayValue
{
    bool enocder;
    bool hallSensor;
    bool engine;
    bool error;
    bool signalLeft;
    bool wifi;
    bool server;
    bool free;

    uint8_t contest;
    uint8_t mark;
};

class DisplayHandler
{
private:
    uint8_t numofSegments[11] = {
        0b00111111, // 0
        0b00000110, // 1
        0b01011011, // 2
        0b01001111, // 3
        0b01100110, // 4
        0b01101101, // 5
        0b01111101, // 6
        0b00000111, // 7
        0b01111111, // 8
        0b01101111, // 9
        0b01001001, // -
    };
    uint8_t _dataPin;
    uint8_t _clockPin;
    uint8_t _latchPin;

    DisplayValue displayVal;
    bool _isLock = false;
    SemaphoreHandle_t _displayMutex; ///< mutex to protect display access
    uint8_t getSegmentNum(uint8_t num);

public:
    DisplayHandler();
    bool begin(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin);
    /**
     * Display show content on the screen.
     */
    void display();
    /**
     * Acquire the display mutex. Timeout in ms (portMAX_DELAY to wait forever).
     * Returns true if the lock was obtained.
     */
    bool lock(uint32_t timeoutMs = portMAX_DELAY);
    /**
     * Release the display mutex.
     */
    void unlock();
    /**
     * Check if the display mutex is currently locked.
     */
    bool isLock();
    /**
     * Show value on display.
     */
    void show(DisplayValue value);

    void clear();
    void setEncoder(bool status);
    void setHallSensor(bool status);
    void setEngine(bool status);
    void setError(bool status);
    void setSignalLeft(bool status);
    void setWiFi(bool status);
    void setServer(bool status);
    void setFree(bool status);
    void setContest(uint8_t contest);
    void setMark(uint8_t mark);
};