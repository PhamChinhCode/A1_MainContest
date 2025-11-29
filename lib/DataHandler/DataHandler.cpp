#include <DataHandler.h>

uint8_t DataHandler::crc8_maxim(const uint8_t *data, size_t len)
{
    uint8_t crc = 0x00; // init
    for (size_t i = 0; i < len; ++i)
    {
        crc ^= data[i]; // XOR với dữ liệu
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x01)
            {
                crc = (crc >> 1) ^ 0x8C; // 0x8C là đa thức đảo bit của 0x31
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}
void DataHandler::dataEncode(uint8_t *data, size_t len)
{
    uint8_t crc = crc8_maxim(data, len);
}
void DataHandler::dataDecode(uint8_t *data, size_t len)
{
    uint8_t crc = crc8_maxim(data, len);
    if (crc != 0x00)
    {
        // Handle error or invalid data
        for (uint8_t i = 0; i < len; i++)
        {
            // Process the decoded data here
        }
    }
}