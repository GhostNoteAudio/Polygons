#pragma once

#include "Arduino.h"
#include <cmath>
#include <algorithm>

namespace Polygons
{
    static inline float Clip1(float value)
    {
        return value > 1 ? 1 : (value < -1 ? -1 : value);
    }

    static inline float ClipF(float value, float min, float max)
    {
        return value > max ? max : (value < min ? min : value);
    }

    static inline int ClipI(int value, int min, int max)
    {
        return value > max ? max : (value < min ? min : value);
    }

    static inline void ZeroBuffer(float* buffer, int len)
    {
        for (int i = 0; i < len; i++)
            buffer[i] = 0.0f;
    }

    static inline void ZeroBuffer(int16_t* buffer, int len)
    {
        for (int i = 0; i < len; i++)
            buffer[i] = 0;
    }

    static inline void Copy(float* dest, float* source, int len)
    {

        memcpy(dest, source, len * sizeof(float));
    }

    static inline void Gain(float* buffer, float gain, int len)
    {
        for (int i = 0; i < len; i++)
        {
            buffer[i] *= gain;
        }
    }

    static inline void Mix(float* target, float* source, float gain, int len)
    {
        for (int i = 0; i < len; i++)
            target[i] += source[i] * gain;
    }

    template<typename T>
    static double DB2gain(T input)
    {
        return std::pow(10, input / 20.0);
    }

    template<typename T>
    static double Gain2DB(T input)
    {
        if (input < 0.0000001)
            return -100000;

        return 20.0f * std::log10(input);
    }

    static inline double Response4Oct(double input)
    {
        return std::min((std::pow(16, input) - 1.0) * 0.066666666667, 1.0);
    }

    static inline double Response2Dec(double input)
    {
        return std::min((std::pow(100, input) - 1.0) * 0.01010101010101, 1.0);
    }

    // Truncates the end of an IR using a cosine window
    static inline void TruncateCos(float* data, int dataSize, float fraction)
    {
        int sample_count = (int)(fraction * dataSize);
        int offset = dataSize - sample_count;
        for (int i = 0; i < dataSize; i++)
        {
            data[offset + i] *= cosf(i / (float)sample_count * M_PI*0.5);
        }
    }

    static inline void ApplyHamming(float* buffer, int M)
    {
        for (int n = 0; n < M; n++)
        {
            float val = 0.42 - 0.5 * cosf(2*M_PI * n / (double)M) + 0.08 * cosf(4 * M_PI * n / (double)M);
            buffer[n] *= val;
        }
    }

    static inline float Sinc(float x)
    {
        return x == 0 ? 1 : sinf(M_PI*x) / (M_PI*x);
    }

    // Note: choose N as an odd number
    static inline void MakeSincFilter(float* buffer, int N, float fmin, float fmax, int Fs)
    {
        fmin = fmin / (Fs*0.5);
        fmax = fmax / (Fs*0.5);

        for (int i = 0; i < N; i++)
        {
            int x = i - N/2;
            float val = fmax * Sinc(x * fmax) - fmin*Sinc(x * fmin);
            buffer[i] = val;
        }

        ApplyHamming(buffer, N);
    }
}
