#pragma once
#include "../Polygons.h"

template<uint S> // S must be power of 2!!
class DelayBlock
{
    int32_t address;
    uint32_t size;
    uint32_t ptr;
    uint32_t sizeMask;
    float data[S];

public:
    inline DelayBlock(int addess)
    {
        this->address = address;
        this->size = S;
        ptr = 0;
        sizeMask = size - 1;
        for (size_t i = 0; i < size; i++)
            data[i] = 0;        
    }

    inline void write(float* source, int count)
    {
        for (int i = 0; i < count; i++)
        {
            uint writePtr = (ptr + i) & sizeMask;
            data[writePtr] = source[i];
        }
    }

    inline void write(int32_t* source, int count)
    {
        float scaler = (float)(1.0 / (double)SAMPLE_32_MAX);
        for (int i = 0; i < count; i++)
        {
            uint writePtr = (ptr + i) & sizeMask;
            data[writePtr] = source[i] * scaler;
        }
    }

    inline void writeAdd(float* source, int count, float gain)
    {
        for (int i = 0; i < count; i++)
        {
            uint writePtr = (ptr + i) & sizeMask;
            data[writePtr] += source[i] * gain;
        }
    }

    inline void updatePtr(int increment)
    {
        ptr = (ptr + increment) & sizeMask;
    }

    inline void read(float* dest, uint32_t delay, int count)
    {
        for (int i = 0; i < count; i++)
        {
            uint readPtr = ((int)ptr - (int)delay + i + size) & sizeMask;
            dest[i] = data[readPtr];
        }
    }

    inline void read(int32_t* dest, uint32_t delay, int count)
    {
        for (int i = 0; i < count; i++)
        {
            uint readPtr = ((int)ptr - (int)delay + i + size) & sizeMask;
            dest[i] = data[readPtr] * SAMPLE_32_MAX;
        }
    }
};