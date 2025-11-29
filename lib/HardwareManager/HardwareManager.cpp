#include "HardwareManager.h"

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
}
void HardwareManager::update()
{
    dateTime.update();
}