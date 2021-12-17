#pragma once

// created with http://t-filter.engineerjs.com/
// used scipy to convert to min phase
// import scipy.signal as sig
// filter_minimum = sig.minimum_phase(filter_taps)
// filter_minimum = sig.convolve(filter_minimum, filter_minimum)
// 48Khz samplerate
// 10k passband, 2db ripple
// 12k stopband, ~60db attenuation
float brickwall10k[47] = { 
    1.8778637971e-02,  9.0682700443e-02,  2.2496738427e-01,
    3.5236026405e-01,  3.5019837609e-01,  1.6810484765e-01,
    -7.5711270130e-02, -1.8063601091e-01, -7.7071506630e-02,
    8.6235963723e-02,  1.1447460971e-01, -1.8843288691e-03,
    -9.4052634639e-02, -4.8029559087e-02,  5.3804464116e-02,
    6.6227938202e-02, -1.4174974519e-02, -6.2293635710e-02,
    -1.5587384376e-02,  4.6030874813e-02,  3.3846911470e-02,
    -1.9056440943e-02, -2.0806708680e-02,  3.5187192927e-02,
    6.7889837573e-02,  3.1428443101e-02, -2.3094645918e-02,
    -2.9675697819e-02,  3.4274289143e-03,  2.1195454606e-02,
    5.1673149559e-03, -1.2801709358e-02, -7.8293782037e-03,
    6.2867284661e-03,  7.4071252007e-03, -1.9663208936e-03,
    -5.6529997867e-03, -4.3400691344e-04,  3.6214997892e-03,
    1.4058310829e-03, -1.8653011187e-03, -1.3632049551e-03,
    9.9271473487e-04,  1.6854051611e-03,  7.6429775735e-04,
    5.9391382760e-05,  1.2742799955e-06
};


constexpr bool powerOf2(int n)
{
    return (n & (n - 1)) == 0;
}

// N must be factor of 2!
template<int N>
class Fir
{
	const int mask = N - 1;
	unsigned int bufIdx = 0;
public:
	float Buffer[N] = { 0 };
	float* Kernel;
	unsigned int KernelSize;

	Fir(float* kernel, unsigned int kernelSize)
	{
        static_assert(powerOf2(N), "N must be power of 2");
        if (kernelSize > N)
        {
            Kernel = 0;
            KernelSize = 0;
            return;
        }

		Kernel = kernel;
		KernelSize = kernelSize;
	}

    void ClearBuffers()
    {
        for (size_t i = 0; i < N; i++)
        {
            Buffer[i] = 0.0;
        }
    }

	float Process(float input)
	{
		Buffer[bufIdx & mask] = input;
		float outputSample = 0;
		for (size_t k = 0; k < KernelSize; k++)
		{
			unsigned int readIdx = bufIdx - k;
			outputSample += Kernel[k] * Buffer[readIdx & mask];
		}

		bufIdx++;
		return outputSample;
	}
};
