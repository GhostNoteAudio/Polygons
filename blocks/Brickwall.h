#pragma once

// created with http://t-filter.engineerjs.com/
// used scipy to convert to min phase
// 48Khz samplerate
// 10k passband, 2db ripple
// 12k stopband, ~75db attenuation
float brickwall10k[55] = { 
    6.7241509587e-03,  4.0963448265e-02,  1.2737649738e-01,
    2.5602600402e-01,  3.5176390649e-01,  3.1343822857e-01,
    1.2076302595e-01, -1.0285958971e-01, -1.8098173335e-01,
    -6.6583440546e-02,  9.3760976316e-02,  1.1852447296e-01,
    -1.6313505088e-04, -9.8699757211e-02, -5.7343484569e-02,
    5.1817253950e-02,  7.6718198218e-02, -4.9795768171e-03,
    -6.8730096770e-02, -2.8933405796e-02,  4.6174352050e-02,
    4.6500734444e-02, -1.9350821409e-02, -4.9112100985e-02,
    -4.2374893218e-03,  4.1280526696e-02,  2.1145577113e-02,
    -2.7201974806e-02, -2.9413026434e-02,  1.2075791081e-02,
    3.0747806903e-02,  2.0305634905e-03, -2.5508203769e-02,
    -1.1261660705e-02,  1.8266919856e-02,  1.7504433524e-02,
    -8.1180825370e-03, -1.7118089296e-02,  1.6707586670e-03,
    1.6848270681e-02,  6.8847918672e-03, -9.5353369388e-03,
    -6.8144244937e-03,  8.9746781325e-03,  1.3521232822e-02,
    2.3211604017e-03, -5.3843151073e-03,  2.8501254069e-03,
    1.5965879513e-02,  1.6979527582e-02,  5.0745366036e-03,
    -6.8139865143e-03, -9.5834650984e-03, -5.6063572309e-03,
    -1.4977829224e-03
};

// N must be factor of 2!
template<int N>
class Fir
{
	const int mask = N - 1;
	unsigned int bufIdx = 0;
public:
	float Buffer[N] = { 0 };
	float* Kernel;
	int KernelSize;

	Fir(float* kernel, int kernelSize)
	{
        if (kernelSize > N)
        {
            Kernel = 0;
            KernelSize = 0;
            return;
        }

		Kernel = kernel;
		KernelSize = kernelSize;
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
