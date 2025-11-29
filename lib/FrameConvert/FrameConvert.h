#pragma once
#include <Arduino.h>
#include <Myconfig.h>
struct Command
{
    uint8_t key;
    uint32_t value;
};
class FrameConvert
{
private:
    uint8_t *frame;
    size_t len;
    uint8_t key;
    uint32_t value;

public:
    FrameConvert();
    void setFrame(uint8_t *data, size_t len);
    void convert();
    uint8_t getKey();
    uint32_t getValue();
    void setValue(uint32_t value);
    void setKey(uint8_t key);
    uint8_t *encode();
    size_t getFrame(uint8_t *data, size_t len);
};