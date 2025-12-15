
#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class SerialHandle
{
private:
    SemaphoreHandle_t serialMutex;

public:
    Stream *_stream; ///< pointer to any Stream (HardwareSerial, USBCDC, etc.)
    SerialHandle();
    ~SerialHandle();
    // Initialize with a HardwareSerial (begin with baud)
    void begin(HardwareSerial &serialPort, uint32_t baudRate);
    // Initialize with any Stream (e.g. USBCDC) - no baud required
    void begin(Stream &stream);
    void print(const String &message);
    void print(const char *message);
    void println(const String &message);
    void println(const char *message);
    void logE(const String &message);
    void logI(const String &message);
    void logO(const String &message);
    void logD(const String &message);
};