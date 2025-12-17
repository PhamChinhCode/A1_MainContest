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
void SerialHandle::write(const uint8_t *buffer, size_t size)
{
    if (serialMutex == NULL || _stream == nullptr)
        return;
    if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE)
    {
        _stream->write(buffer, size);
        xSemaphoreGive(serialMutex);
    }
}
void SerialHandle::sendCommand(Command *cmd)
{
    if (serialMutex == NULL || _stream == nullptr)
        return;

    if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE)
    {
        _stream->write(frameConvert.getFrame(cmd), 10);
        xSemaphoreGive(serialMutex);
    }
}
void SerialHandle::println(const String &message)
{
    if (serialMutex == NULL || _stream == nullptr)
        return;
    if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE)
    {
        _stream->println(message);
        xSemaphoreGive(serialMutex);
    }
}
void SerialHandle::println(const char *message)
{
    if (serialMutex == NULL || _stream == nullptr)
        return;
    if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE)
    {
        _stream->println(message);
        xSemaphoreGive(serialMutex);
    }
}
