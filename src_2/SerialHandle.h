
#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class SerialHandle
{
private:
    Stream *_stream; ///< pointer to any Stream (HardwareSerial, USBCDC, etc.)

    SemaphoreHandle_t serialMutex;

public:
    SerialHandle();
    ~SerialHandle();
    // Initialize with a HardwareSerial (begin with baud)
    void begin(HardwareSerial &serialPort, uint32_t baudRate);
    // Initialize with any Stream (e.g. USBCDC) - no baud required
    void begin(Stream &stream);
    void print(const String &message);
    void print(const char *message);
};