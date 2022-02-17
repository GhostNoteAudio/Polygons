#pragma once
#include "Utils.h"
#include "cmath"

namespace Polygons
{
    class Bitcrusher
    {
    public:
        static void Process(float* input, float* output, int bits, int bufSize)
        {
            // Effect adds mu-law type compression to each sample to increase perceived dynamic range, and preserve low-gain detail.
            float compressPower = 0.5;

            if (bits < 1)
                bits = 1;

            if (bits >= 16)
            {
                Copy(output, input, bufSize);
                return;
            }

            float scaler = (1 << bits);
            auto scalerInv = 1.0f / scaler;

            for (int i = 0; i < bufSize; i++)
            {
                auto inp = input[i];
                auto x = std::pow(std::abs(inp), compressPower);
                x = inp > 0 ? x : -1;
                auto compInv = inp / x;
                output[i] = std::roundf(scaler * x) * scalerInv * compInv;
            }
        }
    };
}