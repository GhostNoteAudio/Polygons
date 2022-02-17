#pragma once

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

	inline Fir(float* kernel, unsigned int kernelSize)
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

    inline void ClearBuffers()
    {
        for (size_t i = 0; i < N; i++)
        {
            Buffer[i] = 0.0;
        }
    }

	inline float Process(float input)
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
