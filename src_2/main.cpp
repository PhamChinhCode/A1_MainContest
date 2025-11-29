#define LED_PIN 2

#include <FreeRTOS.h>
#include <Arduino.h>
#include "SerialHandle.h"

TaskHandle_t testSDTaskHandle = NULL;
TaskHandle_t mainCode = NULL;
SerialHandle logS;

void testSD(void *pvParameters)
{
    while (1)
    {
        logS.print("core 0: --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}
void maincode(void *pvParameters)
{
    while (1)
    {
        logS.print("core 1: |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
void setup()
{

    logS.begin(Serial0, 4800);
    pinMode(LED_PIN, OUTPUT);

    xTaskCreatePinnedToCore(
        testSD,            /* Function to implement the task */
        "Test SD Task",    /* Name of the task */
        4096,              /* Stack size in words */
        NULL,              /* Task input parameter */
        1,                 /* Priority of the task */
        &testSDTaskHandle, /* Task handle. */
        0);                /* Core where the task should run */
    xTaskCreatePinnedToCore(
        maincode,         /* Function to implement the task */
        "Main Code Task", /* Name of the task */
        4096,             /* Stack size in words */
        NULL,             /* Task input parameter */
        1,                /* Priority of the task */
        &mainCode,        /* Task handle. */
        1);               /* Core where the task should run */
}
void loop()
{
}