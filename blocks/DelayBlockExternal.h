#pragma once
#include "Polygons.h"

template<uint S, uint B> // S must be power of 2, S must be multiple of B
class DelayBlockExternal
{
    int32_t address;
    uint32_t size;
    uint32_t ptr;
    uint32_t sizeMask;
    uint32_t blocksize;
    uint8_t buffer[B*4];

public:
    inline DelayBlockExternal(int addess)
    {
        this->address = address;
        this->size = S;
        this->blocksize = B;
        ptr = 0;
        sizeMask = size - 1;
    }

    inline void init()
    {
        CS = P_SPI_SRAM_CS;
        SPI.begin();
        int32_t zeros[64] = { 0 };
        for (size_t i = 0; i < size / 64; i++)
        {
            write(zeros, 64);
            updatePtr(64);
        }
    }

    inline void writeBuffer(int count)
    {
        Polygons::sram.WriteByteArray(address + ptr * 4, (uint8_t*)buffer, count * 4);
    }

    inline void write(int32_t* source, int count)
    {
        memcpy(buffer, (uint8_t*)source, count*4);
        writeBuffer(count);
    }

    inline void write(float* source, int count)
    {
        for (int i = 0; i < count; i++)
            buffer[i] = source[i] * SAMPLE_32_MAX;
        writeBuffer(count);
    }

    inline void updatePtr(int increment)
    {
        ptr = (ptr + increment) & sizeMask;
    }

    inline void read(int32_t* dest, uint32_t delay, int count)
    {
        uint readPtr = ((int)ptr - (int)delay + size) & sizeMask;
        uint endAddress = readPtr + count;
        if (endAddress >= size)
        {
            // reading across the wrapping boundary, need two reads
            uint count1 = endAddress - size;
            uint count2 = count - count1;
            uint readPtr2 = (readPtr + count1) & sizeMask;
            Polygons::sram.ReadByteArray(address + readPtr * 4, (uint8_t*)dest, count1 * 4);
            Polygons::sram.ReadByteArray(address + readPtr2 * 4, (uint8_t*)&dest[count1], count2 * 4);
        }
        else
        {
            Polygons::sram.ReadByteArray(address + readPtr * 4, (uint8_t*)dest, count * 4);
        }
    }
};
