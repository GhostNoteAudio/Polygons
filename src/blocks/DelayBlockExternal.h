#pragma once
#include "Polygons.h"
#include "Utils.h"

constexpr bool powerOf2(int n)
{
    return (n & (n - 1)) == 0;
}

// The SRAM on Polygons has 64 megabits = 8 megabytes of storage.
// this translates to 2 million floats or 4 million 16 bits samples
template<uint S, uint B> // S must be multiple of B
class DelayBlockExternal
{
    int32_t address;
    uint32_t ptr;
    float txBuffer[B]; // needed because the transmit function blats the existing data in the buffer!

public:
    inline DelayBlockExternal(int addess)
    {
        static_assert(S % B == 0, "S must be multiple of B");   
        this->address = address;
        ptr = 0;
    }

    inline void init()
    {
        CS = P_SPI_SRAM_CS;
        SPI.begin();
        // float zero[1] = { 0.0 };
        // for (size_t i = 0; i < S; i++)
        // {
        //     writeToRam(zero, 1);
        // }
    }

    inline void writeToRam(float* data, int count)
    {
        SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
        Polygons::sram.WriteByteArray(address + ptr * 4, (uint8_t*)data, count * 4);
        SPI.endTransaction();
    }

    inline void write(float* source, int bufSize)
    {
        Polygons::Copy(txBuffer, source, bufSize);
        writeToRam(txBuffer, bufSize);
    }

    inline void updatePtr(int increment)
    {
        ptr = (ptr + increment) % S;
    }

    inline void setPtr(int position)
    {
        ptr = position;
    }

    inline int getPtr()
    {
        return ptr;
    }

    inline void read(float* dest, uint32_t delay, int count)
    {
        SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
        uint readPtr = ((int)ptr - (int)delay + S) % S;
        uint endAddress = readPtr + count;
        if (endAddress >= S)
        {
            // reading across the wrapping boundary, need two reads
            uint count1 = endAddress - S;
            uint count2 = count - count1;
            uint readPtr2 = (readPtr + count1) % S;
            Polygons::sram.ReadByteArray(address + readPtr * 4, (uint8_t*)dest, count1 * 4);
            Polygons::sram.ReadByteArray(address + readPtr2 * 4, (uint8_t*)&dest[count1], count2 * 4);
        }
        else
        {
            Polygons::sram.ReadByteArray(address + readPtr * 4, (uint8_t*)dest, count * 4);
        }
        SPI.endTransaction();
    }
};
