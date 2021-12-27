#pragma once

inline float InterpolateSpline(float* data, int dLen, float pos)
{
	double y0, y1, y2, y3;
	double a, b, c, mu;
	double output;

	int x0 = (int)pos - 1;
	mu = pos - x0 - 1;

	// If pos is an integer, or close to it, return exact index
	if (mu == 0.0f)
	{
		output = data[x0 + 1];
		return output;
	}

	// Find x0
	if (x0 < 0)
		y0 = data[x0 + 1];
	else
		y0 = data[x0];

	y1 = data[x0 + 1];
	y2 = data[x0 + 2];

	// Find x3
	if (x0 + 3 > (dLen - 1))
		y3 = data[x0 + 2];
	else
		y3 = data[x0 + 3];

	// Spline 1
	c = y1;
	a = (y2 + y0) * 0.5f - y1;
	b = (y2 - y0) * 0.5f;
	double v1 = a * mu * mu + b * mu + c;

	// Spline 2
	c = y2;
	a = (y3 + y1) / 2 - y2;
	b = (y3 - y1) / (2);
	double mmu = (mu - 1);
	double v2 = a * mmu * mmu + b * mmu + c;

	output = v1 * (1 - mu) + v2 * mu;
	return output;
}

inline int ScaleInPlace(float* data, int kSize, int maxNewLen, float scale)
{
	if (scale == 1)
		return kSize;

    float scaleInv = 1 / scale;
    if (scale < 1) // stretching, making the data longer
	{
		int newLen = kSize * scaleInv;
        if (newLen > maxNewLen)
            newLen = maxNewLen;
		for (int i = newLen; i >= 0; i--)
		{
			float sourceIdx = i * scale;
			float newSample = InterpolateSpline(data, kSize, sourceIdx);
			data[i] = newSample;
		}
		return newLen;
	}
	else // if (scale > 1) // compacting, making the data shorter
	{
		int newLen = kSize * scaleInv;
		for (int i = 0; i < newLen; i++)
		{
			float sourceIdx = i * scale;
			float newSample = InterpolateSpline(data, kSize, sourceIdx);
			data[i] = newSample;
		}
		return newLen;
	}
}
