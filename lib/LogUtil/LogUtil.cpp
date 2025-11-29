#include "LogUtil.h"

SemaphoreHandle_t serialMutex = xSemaphoreCreateMutex();

void logSerial(String message)
{
    xSemaphoreTake(serialMutex, portMAX_DELAY);
    SERIAL_LOG.println(message);
    xSemaphoreGive(serialMutex);
}