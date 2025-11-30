#include "DisplayHandler.h"
#include <defineName.h>
#include <Arduino.h>

DisplayHandler::DisplayHandler()
{
    _displayMutex = xSemaphoreCreateMutex();
}
bool DisplayHandler::begin(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin)
{
    _dataPin = dataPin;
    _clockPin = clockPin;
    _latchPin = latchPin;

    pinMode(_dataPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    pinMode(_latchPin, OUTPUT);

    clear();
    display();

    return true;
}
void DisplayHandler::display()
{

    uint8_t data1 = 0;
    uint32_t data2 = 0;
    uint8_t contestNo = displayVal.contest == STATE_CONTEST1_RUNNING   ? 1
                        : displayVal.contest == STATE_CONTEST2_RUNNING ? 2
                        : displayVal.contest == STATE_CONTEST3_RUNNING ? 3
                        : displayVal.contest == STATE_CONTEST4_RUNNING ? 4
                        : displayVal.contest == STATE_READY_COMMAND    ? 0
                        : displayVal.contest == STATE_FREE_COMMAND     ? 10
                                                                       : 8;
    // Build data word
    // data |= (display.enocder ? 1 : 0) << 15;
    data1 |= (displayVal.enocder ? 1 : 0) << 7;
    data1 |= (displayVal.hallSensor ? 1 : 0) << 6;
    data1 |= (displayVal.engine ? 1 : 0) << 5;
    data1 |= (displayVal.signalLeft ? 1 : 0) << 4;
    data1 |= (displayVal.wifi ? 1 : 0) << 3;
    data1 |= (displayVal.server ? 1 : 0) << 2;
    data1 |= (displayVal.free ? 1 : 0) << 1;
    data1 |= (displayVal.error ? 1 : 0);

    data2 |= getSegmentNum(contestNo) << 24;
    data2 |= getSegmentNum(displayVal.mark / 100) << 16;
    data2 |= getSegmentNum((displayVal.mark % 100) / 10) << 8;
    data2 |= getSegmentNum(displayVal.mark % 10);

    if (xSemaphoreTake(_displayMutex, portMAX_DELAY) == pdTRUE)
    { // Shift out data
        digitalWrite(_latchPin, LOW);

        shiftOut(_dataPin, _clockPin, LSBFIRST, data1);
        shiftOut(_dataPin, _clockPin, LSBFIRST, data2 & 0xFF);
        shiftOut(_dataPin, _clockPin, LSBFIRST, (data2 >> 8) & 0xFF);
        shiftOut(_dataPin, _clockPin, LSBFIRST, (data2 >> 16) & 0xFF);
        shiftOut(_dataPin, _clockPin, LSBFIRST, (data2 >> 24) & 0xFF);

        digitalWrite(_latchPin, HIGH);
        xSemaphoreGive(_displayMutex);
    }
}
uint8_t DisplayHandler::getSegmentNum(uint8_t num)
{
    return numofSegments[num];
}
void DisplayHandler::clear()
{
    displayVal.contest = 0;
    displayVal.mark = 0;
    displayVal.enocder = false;
    displayVal.hallSensor = false;
    displayVal.engine = false;
    displayVal.error = false;
    displayVal.signalLeft = false;
    displayVal.wifi = false;
    displayVal.server = false;
    displayVal.free = false;
    display();
}
bool DisplayHandler::lock(uint32_t timeoutMs)
{
    if (xSemaphoreTake(_displayMutex, pdMS_TO_TICKS(timeoutMs)) == pdTRUE)
    {
        _isLock = true;
        return true;
    }
    return false;
}
void DisplayHandler::unlock()
{
    xSemaphoreGive(_displayMutex);
    _isLock = false;
}
bool DisplayHandler::isLock()
{
    return _isLock;
}
void DisplayHandler::show(DisplayValue value)
{
    displayVal = value;
    display();
}
void DisplayHandler::setEncoder(bool status)
{
    displayVal.enocder = status;
}
void DisplayHandler::setHallSensor(bool status)
{
    displayVal.hallSensor = status;
}
void DisplayHandler::setEngine(bool status)
{
    displayVal.engine = status;
}
void DisplayHandler::setError(bool status)
{
    displayVal.error = status;
}
void DisplayHandler::setSignalLeft(bool status)
{
    displayVal.signalLeft = status;
}
void DisplayHandler::setWiFi(bool status)
{
    displayVal.wifi = status;
}
void DisplayHandler::setServer(bool status)
{
    displayVal.server = status;
}
void DisplayHandler::setFree(bool status)
{
    displayVal.free = status;
}
void DisplayHandler::setContest(uint8_t contest)
{
    displayVal.contest = contest;
}
void DisplayHandler::setMark(uint8_t mark)
{
    displayVal.mark = mark;
}