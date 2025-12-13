#pragma once
#include <Arduino.h>
#include <FrameConvert.h>
#include <defineName.h>
#include <SerialHandle.h>
struct ScreenData
{
    uint32_t timeSpand;
    uint32_t IdNumber;
    uint8_t score;
    uint8_t contestStatus;
    bool wifiStatus;
    bool softWare;
    bool engineStatus;
};

class ScreenHandler
{
private:
    SerialHandle serialScreen;

public:
    ScreenHandler(/* args */);
    ~ScreenHandler();
};
