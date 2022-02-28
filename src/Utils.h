#pragma once

#include "Arduino.h"
#include "AudioConfig.h"
#include <cmath>
#include <algorithm>

namespace Polygons
{
    inline float Clip1(float value)
    {
        return value > 1 ? 1 : (value < -1 ? -1 : value);
    }

    inline float ClipF(float value, float min, float max)
    {
        return value > max ? max : (value < min ? min : value);
    }

    inline int ClipI(int value, int min, int max)
    {
        return value > max ? max : (value < min ? min : value);
    }

    inline void ZeroBuffer(float* buffer, int len)
    {
        for (int i = 0; i < len; i++)
            buffer[i] = 0.0f;
    }

    inline void ZeroBuffer(int16_t* buffer, int len)
    {
        for (int i = 0; i < len; i++)
            buffer[i] = 0;
    }

    inline void Copy(float* dest, float* source, int len)
    {

        memcpy(dest, source, len * sizeof(float));
    }

    inline void Gain(float* buffer, float gain, int len)
    {
        for (int i = 0; i < len; i++)
        {
            buffer[i] *= gain;
        }
    }

    inline void Mix(float* target, float* source, float gain, int len)
    {
        for (int i = 0; i < len; i++)
            target[i] += source[i] * gain;
    }

    inline void IntBuffer2Float(float* dest, int32_t* source, int len)
    {
        float scaler = (float)(1.0 / (double)SAMPLE_32_MAX);
        for (int i = 0; i < len; i++)
            dest[i] = source[i] * scaler;
    }

    inline void FloatBuffer2Int(int32_t* dest, float* source, int len)
    {
        for (int i = 0; i < len; i++)
            dest[i] = (int)(source[i] * SAMPLE_32_MAX);
    }

    inline float MaxAbsF(float* data, int len)
    {
        float max = -99999999;
        for (int i = 0; i < len; i++)
        {
            float val = fabsf(data[i]);
            max = val > max ? val : max;
        }
        return max;
    }

    template<typename T>
    double DB2gain(T input)
    {
        return std::pow(10, input / 20.0);
    }

    template<typename T>
    double Gain2DB(T input)
    {
        if (input < 0.0000001)
            return -100000;

        return 20.0f * std::log10(input);
    }

    inline double Response4Oct(double input)
    {
        return std::min((std::pow(16, input) - 1.0) * 0.066666666667, 1.0);
    }

    inline double Response2Dec(double input)
    {
        return std::min((std::pow(100, input) - 1.0) * 0.01010101010101, 1.0);
    }

    // Truncates the end of an IR using a cosine window
    inline void TruncateCos(float* data, int dataSize, float fraction)
    {
        int sample_count = (int)(fraction * dataSize);
        int offset = dataSize - sample_count;
        for (int i = 0; i < dataSize; i++)
        {
            data[offset + i] *= cosf(i / (float)sample_count * M_PI*0.5);
        }
    }

    inline void ApplyHamming(float* buffer, int M)
    {
        for (int n = 0; n < M; n++)
        {
            float val = 0.42 - 0.5 * cosf(2*M_PI * n / (double)M) + 0.08 * cosf(4 * M_PI * n / (double)M);
            buffer[n] *= val;
        }
    }

    inline float Sinc(float x)
    {
        return x == 0 ? 1 : sinf(M_PI*x) / (M_PI*x);
    }

    // Note: choose N as an odd number
    inline void MakeSincFilter(float* buffer, int N, float fmin, float fmax, int Fs)
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
