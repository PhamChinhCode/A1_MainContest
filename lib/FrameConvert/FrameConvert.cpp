#include <Arduino.h>
#include <Myconfig.h>
#include <defineName.h>
#include "FrameConvert.h"

FrameConvert::FrameConvert()
{
    frame = nullptr;
    len = 0;
    key = 0;
    value = 0;
}

void FrameConvert::setFrame(uint8_t *data, size_t len)
{
    frame = data;
    this->len = len;
    key = KEY_NULL;
    value = KEY_NULL;

    convert();
}

uint8_t FrameConvert::getKey()
{
    return key;
}

uint32_t FrameConvert::getValue()
{
    return value;
}

void FrameConvert::setValue(uint32_t value)
{
    this->value = value;
}

void FrameConvert::setKey(uint8_t key)
{
    this->key = key;
}

void FrameConvert::convert()
{
    if (frame == nullptr)
        return;
    if (len < 8)
        return;

    if (frame[0] != BYTE_START)
        return;

    // frame type: GET/SET nằm ở byte 2 hoặc 3 tùy bạn thiết kế
    if (frame[2] != BYTE_GET && frame[2] != BYTE_SET)
        return;

    key = frame[1];

    value =
        ((uint32_t)frame[4] << 24) |
        ((uint32_t)frame[5] << 16) |
        ((uint32_t)frame[6] << 8) |
        (uint32_t)frame[7];
}

uint8_t *FrameConvert::Encode()
{

    frame[0] = BYTE_START;
    frame[1] = key;
    frame[2] = BYTE_GET; // hoặc set tùy bạn
    frame[3] = 0;
    frame[4] = (value >> 24) & 0xFF;
    frame[5] = (value >> 16) & 0xFF;
    frame[6] = (value >> 8) & 0xFF;
    frame[7] = value & 0xFF;
    frame[8] = BYTE_STOP;
    frame[9] = 0; // CRC hoặc byte kiểm tra tùy bạn
    return frame;
}
Command *FrameConvert::getCommand(uint8_t *data, size_t len)
{
    if (data == nullptr)
        return nullptr;

    if (data[0] != BYTE_START)
        return nullptr;
    if (data[2] != BYTE_GET && data[2] != BYTE_SET)
        return nullptr;

    decode.key = data[1];

    decode.value = ((uint32_t)data[4] << 24) |
                   ((uint32_t)data[5] << 16) |
                   ((uint32_t)data[6] << 8) |
                   (uint32_t)data[7];

    return &decode;
}
uint8_t *FrameConvert::getFrame(Command *cmd, uint8_t type, uint8_t id, uint8_t StopByte)
{
    encodeFrame[0] = BYTE_START;
    encodeFrame[1] = cmd->key;
    encodeFrame[2] = type;
    encodeFrame[3] = id;

    encodeFrame[4] = (cmd->value >> 24) & 0xFF;
    encodeFrame[5] = (cmd->value >> 16) & 0xFF;
    encodeFrame[6] = (cmd->value >> 8) & 0xFF;
    encodeFrame[7] = cmd->value & 0xFF;

    encodeFrame[8] = 0;
    encodeFrame[9] = StopByte;
    return encodeFrame;
}