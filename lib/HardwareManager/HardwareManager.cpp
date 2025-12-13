#include "HardwareManager.h"
#include <Myconfig.h>

HardwareManager::HardwareManager()
{
}
HardwareManager::~HardwareManager()
{
}
void HardwareManager::begin()
{
    microSD.begin();
    camera.init();
    serialLog.begin(SERIAL_OUTPUT, SERIAL_BAUDRATE);
    display.begin(DATA_PIN_DISPLAY, CLOCK_PIN_DISPLAY, LATCH_PIN_DISPLAY);
}
void HardwareManager::update()
{
    dateTime.update();
}
