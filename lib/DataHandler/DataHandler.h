#pragma once
#include <Myconfig.h>
#include <Arduino.h>

class DataHandler
{
private:
    /* data */
    uint8_t crc8_maxim(const uint8_t *data, size_t len);

public:
    DataHandler(/* args */);
    ~DataHandler();
    void dataEncode(uint8_t *data, size_t len);
    void dataDecode(uint8_t *data, size_t len);
};
