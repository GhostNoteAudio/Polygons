#include "arm_math.h"

struct Cplx
{
	float Real;
	float Imag;
};

void ComplexMultiply(Cplx& dest, Cplx& c1, Cplx& c2)
{
	float r = c1.Real * c2.Real - c1.Imag * c2.Imag;
	float i = c1.Real * c2.Imag + c1.Imag * c2.Real;
	dest.Real = r;
	dest.Imag = i;
}
 
template<int N>
class ConvFFT
{
	float outputBuffer[N];
	Cplx K[N] __attribute__ ((aligned (4))); // complex values, real and imag interleaved
	Cplx X[N] __attribute__ ((aligned (4))); // complex values, real and imag interleaved
	unsigned int maxBlockSize;
	unsigned int outputIndex;
	arm_cfft_radix2_instance_f32 fftinst;
	arm_cfft_radix2_instance_f32 ifftinst;
public:

	ConvFFT(unsigned int maxBlockSize)
	{
		this->maxBlockSize = maxBlockSize;
		outputIndex = 0;
		arm_cfft_radix2_init_f32(&fftinst, N, 0, 1);
    	arm_cfft_radix2_init_f32(&ifftinst, N, 1, 1);
	}

	void SetKernel(float* kernel, unsigned int kernelSize)
	{
		// NOTE: kernel can be at max N - maxBlockSize long, otherwise circular wrap-around would occur!
		if (kernelSize > N - maxBlockSize)
		{
			Serial.println("Kernel can have maximum length of N - maxBlockSize to prevent circular convolution! Use a larger value N if you need a longer kernel");
			return;
		}

		for (size_t i = 0; i < N; i++)
		{
			K[i].Real = kernel[i];
			K[i].Imag = 0;
			X[i].Real = 0;
			X[i].Imag = 0;
			outputBuffer[i] = 0;
		}

		arm_cfft_radix2_f32(&fftinst, (float*)K);
	}

	void Process(float* input, float* output, unsigned int bufSize)
	{
		Timers::Lap(1);
		if (bufSize > maxBlockSize)
		{
			Serial.println("Trying to process a buffer that's too big!");
			return;
		}

		for (size_t i = 0; i < bufSize; i++)
		{
			X[i].Real = input[i];
			X[i].Imag = 0;
		}
		for (size_t i = bufSize; i < N; i++)
		{
			X[i].Real = 0;
			X[i].Imag = 0;
		}

		Timers::Lap(2);
		arm_cfft_radix2_f32(&fftinst, (float*)X);
		Timers::Lap(3);
		for (size_t i = 0; i < N; i++)
		{
			ComplexMultiply(X[i], X[i], K[i]);
			//X[i] = X[i] * K[i];
		}
		
		arm_cfft_radix2_f32(&ifftinst, (float*)X);
		Timers::Lap(4);
		// clear out old data
		for (size_t i = 0; i < bufSize; i++)
		{
			outputBuffer[(outputIndex - i - 1 + N) % N] = 0;
		}

		for (size_t i = 0; i < N; i++)
		{
			outputBuffer[(outputIndex + i) % N] += X[i].Real;
		}

		for (size_t i = 0; i < bufSize; i++)
		{
			output[i] = outputBuffer[(outputIndex + i) % N];
		}

		outputIndex += bufSize;
		Timers::Lap(5);
	}
};