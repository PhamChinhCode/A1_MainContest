#include "SerialHandle.h"

SerialHandle::SerialHandle()
{
    serialMutex = xSemaphoreCreateMutex();
    _stream = nullptr;
}

void SerialHandle::begin(HardwareSerial &serialPort, uint32_t baudRate)
{
    serialPort.begin(baudRate);
    _stream = &serialPort;
}
void SerialHandle::begin(Stream &stream)
{
    _stream = &stream;
}
SerialHandle::~SerialHandle()
{
    if (serialMutex != NULL)
    {
        vSemaphoreDelete(serialMutex);
    }
}
void SerialHandle::print(const String &message)
{
    if (serialMutex == NULL || _stream == nullptr)
        return;
    if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE)
    {
        _stream->print(message);
        xSemaphoreGive(serialMutex);
    }
}
void SerialHandle::print(const char *message)
{
    if (serialMutex == NULL || _stream == nullptr)
        return;
    if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE)
    {
        _stream->print(message);
        xSemaphoreGive(serialMutex);
    }
}
