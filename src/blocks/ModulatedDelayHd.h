#pragma once

#include "Utils.h"
#include <stdint.h>

namespace Polygons
{
	template<int DelayBufferSizeSamples, int BUF_SIZE>
	class ModulatedDelayHd
	{
	private:

		static const int ModulationUpdateRate = 8;
		static constexpr float SAMPLE_MAX16_INV = (float)(1.0 / INT16_MAX);

		float delayBuffer[DelayBufferSizeSamples];
		float output[BUF_SIZE];
		int writeIndex;
		int readIndexA;
		int readIndexB;
		int samplesProcessed;
		

		double modPhase;
		double gainA;
		double gainB;

	public:
		int SampleDelay;
		double ModAmount; // Sample excursion
		double ModRate; // Frequency / Samplerate

		ModulatedDelayHd()
		{
			SampleDelay = 1000;
			writeIndex = 0;
			modPhase = 0.0;
			ModRate = 0.0;
			ModAmount = 0.0;
			ClearBuffers();
			Update();
		}

		float* GetOutput()
		{
			return output;
		}

		FLASHMEM void Process(float* input, int sampleCount)
		{
			for (int i = 0; i < sampleCount; i++)
			{
				if (samplesProcessed == ModulationUpdateRate)
					Update();

				delayBuffer[writeIndex] = input[i];
				output[i] = delayBuffer[readIndexA] * gainA + delayBuffer[readIndexB] * gainB;

				writeIndex++;
				readIndexA++;
				readIndexB++;
				if (writeIndex >= DelayBufferSizeSamples) writeIndex -= DelayBufferSizeSamples;
				if (readIndexA >= DelayBufferSizeSamples) readIndexA -= DelayBufferSizeSamples;
				if (readIndexB >= DelayBufferSizeSamples) readIndexB -= DelayBufferSizeSamples;
				samplesProcessed++;
			}
		}

		FLASHMEM void ClearBuffers()
		{
			ZeroBuffer(delayBuffer, DelayBufferSizeSamples);
			ZeroBuffer(output, BUF_SIZE);
		}

		FLASHMEM void ResetPhase(float phase)
		{
			this->modPhase = fmodf(phase, 1.0f);
		}

	private:
		FLASHMEM void Update()
		{
			modPhase += ModRate * ModulationUpdateRate;
			if (modPhase > 1)
				modPhase -= 1.0;

			auto mod = sinf(modPhase * 2 * M_PI);

			if (ModAmount >= SampleDelay) // don't modulate to negative value
				ModAmount = SampleDelay - 1;

			auto totalDelay = SampleDelay + ModAmount * mod;

			auto delayA = (int)totalDelay;
			auto delayB = (int)totalDelay + 1;

			auto partial = totalDelay - delayA;

			gainA = 1 - partial;
			gainB = partial;

			readIndexA = writeIndex - delayA;
			readIndexB = writeIndex - delayB;
			if (readIndexA < 0) readIndexA += DelayBufferSizeSamples;
			if (readIndexB < 0) readIndexB += DelayBufferSizeSamples;

			samplesProcessed = 0;
		}
	};
}
