#pragma once
#include "Arduino.h"

namespace Polygons
{
    template<int N>
    class Convolver
    {
        float inputBuffer[N];
        float* kernel;
        int kernelSize;
        int bufferSize;
        int ptr;

        int MASK = N-1;

    public:
        Convolver()
        {
            static_assert((N & (N - 1)) == 0, "N must be power of 2");
        }
        inline void Init(float* kernel, int kernelSize, int bufferSize)
        {
            ptr = 0;
            this->kernel = kernel;
            this->kernelSize = kernelSize;
            this->bufferSize = bufferSize;
            if (N < kernelSize + bufferSize)
            {
                Serial.print("N must be greater or equal to kernelSize + bufferSize. N: ");
                Serial.print(N);
                Serial.print(" kernelSize: ");
                Serial.print(kernelSize);
                Serial.print(" bufferSize: ");
                Serial.println(bufferSize);
            }
        }

        inline void Process(float* input, float* output, int bufSize)
        {
            for (int i = 0; i < bufSize; i++)
            {
                inputBuffer[(ptr + i) & MASK] = input[i];
            }

            for (int i = 0; i < bufSize; i++)
            {
                float val = 0.0;
                int ptri = ptr + i;
                for (int n = 0; n < kernelSize; n++)
                {
                    val += inputBuffer[(ptri - n) & MASK] * kernel[n];
                }
                
                output[i] = val;
            }
            
            ptr = (ptr + bufSize) & MASK; 
        }
    };
}
