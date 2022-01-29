#pragma once
#include "../Polygons.h"
#include "Arduino.h"
#include "DelayBlock.h"

namespace Polygons
{
    class Limiter
    {
    public:
        const int ParameterThreshold = 0;
        const int ParameterBoost = 1;
        const int ParameterRelease = 2;
        const int ParameterRatio = 3;

        static float InputGainDb;
        static float GainReductionDb;
        
    private:
        int samplerate;
        float buffer[256];
        DelayBlock<256> delay;
        float releaseMs;
        int lookaheadSamples;
        float releasePerSample;
        float threshold;
        float ratio;
        float gain;

        float envVal;
        float filteredEnvValSummed;
        int ptr;
        int bufMask;

    public:
        inline Limiter(int samplerate)
        {
            this->samplerate = samplerate;
            releaseMs = 60;
            releasePerSample = DB2gain(-1000/releaseMs*12/samplerate);
            threshold = DB2gain(-3);
            gain = 1.0;
            ratio = 0.5; // 0....1 value, 0 being 1:1, 1 being infinite
            envVal = 0.0;
            ptr = 0;

            // set lookahead to ~2.66ms
            if (samplerate <= 48000)
                lookaheadSamples = 128;
            else // assume 88.1Khz or 96Khz
                lookaheadSamples = 256;

            bufMask = lookaheadSamples - 1;
            ZeroBuffer(buffer, 256);
            filteredEnvValSummed = 0;
        }

        inline void SetParameter(int param, double value)
        {
            if (param < 0 || param > ParameterRatio)
                return;

            if (param == ParameterThreshold)
                threshold = DB2gain(value);
            else if (param == ParameterBoost)
                gain = DB2gain(value);
            else if (param == ParameterRelease)
            {
                releaseMs = value;
                releasePerSample = DB2gain(-1000/releaseMs*12/samplerate);
            }
            else if (param == ParameterRatio)
                ratio = value;
        }

        inline void Process(float* input, float* output, int bufSize)
        {
            auto bufItem = Buffers::Request();
            auto delayBuf = bufItem.Ptr;

            delay.write(input, bufSize);
            delay.read(delayBuf, lookaheadSamples, bufSize);
            delay.updatePtr(bufSize);

            float scaler = 1.0 / lookaheadSamples;
            float thresInv = 1.0 / threshold;

            for (int i = 0; i < bufSize; i++)
            {
                // apply peak detector with decay controlled by the releaseMs param
                auto d = fabsf(input[i]);
                envVal *= releasePerSample;
                if (d > envVal)
                    envVal = d;
                buffer[ptr] = envVal;
                
                auto ptrNext = (ptr + 1) & bufMask;
                // apply moving average filter of size lookaheadSamples
                filteredEnvValSummed += buffer[ptr];
                filteredEnvValSummed -= buffer[ptrNext];
                ptr = ptrNext;
                float filteredEnvVal = filteredEnvValSummed * scaler;

                // actually apply the gain reduction
                auto excessGain = filteredEnvVal * gain * thresInv;
                auto gainReduction = (max(excessGain, 1.0)-1) * ratio + 1;
                auto dataShifted = delayBuf[i];
                output[i] = dataShifted * gain / gainReduction;

                if (i == bufSize - 1) // used for readouts
                {
                    Limiter::InputGainDb = Limiter::InputGainDb * 0.9 + Gain2DB(filteredEnvVal) * 0.1;
                    Limiter::GainReductionDb = Limiter::GainReductionDb * 0.9 + Gain2DB(gainReduction) * 0.1;
                }
            }            
        }
    };
}
