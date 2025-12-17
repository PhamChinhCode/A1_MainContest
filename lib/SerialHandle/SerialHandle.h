
#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <FrameConvert.h>

class SerialHandle
{
private:
    SemaphoreHandle_t serialMutex;
    FrameConvert frameConvert;

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
    void sendCommand(Command *cmd);
    void write(const uint8_t *buffer, size_t size);
    void logE(const String &message, int logType);
    void logI(const String &message, int logType);
    void logO(const String &message, int logType);
    void logD(const String &message, int logType);
};