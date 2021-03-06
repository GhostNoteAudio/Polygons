#pragma once

namespace Polygons
{
	class Biquad
	{
	public:
		enum class FilterType
		{
			LowPass6db = 0,
			HighPass6db,
			LowPass,
			HighPass,
			BandPass,
			Notch,
			Peak,
			LowShelf,
			HighShelf
		};

	private:
		float samplerate;
		float _gainDb;
		float _q;
		float a0, a1, a2, b0, b1, b2;
		float x1, x2, y, y1, y2;
		float gain;
		float aVec[3];
		float bVec[3];

	public:
		FilterType Type;
		float Output;
		float Frequency;

		Biquad();
		Biquad(FilterType filterType, float samplerate);

		float GetSamplerate();
		void SetSamplerate(float samplerate);
		float GetGainDb();
		void SetGainDb(float value);
		float GetGain();
		void SetGain(float value);
		float GetQ();
		void SetQ(float value);
		float* GetA();
		float* GetB();

		void Update();
		float GetResponse(float freq);
		
		float inline Process(float x)
		{
			y = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
			x2 = x1;
			y2 = y1;
			x1 = x;
			y1 = y;

			Output = y;
			return Output;
		}

		void inline Process(float* input, float* output, int len)
		{
			for (int i = 0; i < len; i++)
				output[i] = Process(input[i]);
		}

		void ClearBuffers();
	};
}
