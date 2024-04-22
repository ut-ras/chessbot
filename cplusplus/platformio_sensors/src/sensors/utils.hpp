#include <stdint.h>
typedef uint8_t byte;

#pragma once

static uint8_t processCRC(uint8_t *buffer, uint8_t NUM_BYTES_READING)
{
    // this function is from CHATGPT ftw, let's see if it works
    // CRC-8 polynomial used by TMAG5273 IC: x^8 + x^2 + x^1 + 1
    constexpr uint8_t CRC_POLYNOMIAL = 0x07;

    uint8_t crc = 0xFF; // Initial value

    for (uint8_t i = 0; i < NUM_BYTES_READING - 1; ++i)
    {
        crc ^= buffer[i];
        for (int j = 0; j < 8; ++j)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ CRC_POLYNOMIAL;
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}
