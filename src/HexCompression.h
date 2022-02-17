#pragma once
#include <stdint.h>

const char HexTable[16] =
{
	'0',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'A',
	'B',
	'C',
	'D',
	'E',
	'F'
};

inline uint8_t GetHexValue(const char value)
{
	if (value >= '0' && value <= '9')
		return value - '0';
	if (value >= 'A' && value <= 'F')
		return value - 'A' + 10;
	return 0;
}

// Converts a byte array into a string representation, using hexadecimal numbers and a VERY rudimentary compression algorithm
int Compress(const uint8_t* data, int dataSize, char* dest)
{
	uint8_t current = data[0];
	int currentCount = 0;
	int writePtr = 0;

	auto Emit = [&]() 
	{
		if (currentCount == 0)
			return;
		else if (currentCount == 1)
		{
			auto hi = HexTable[current >> 4];
			auto lo = HexTable[current & 0x0F];
			dest[writePtr] = hi;
			dest[writePtr + 1] = lo;
			writePtr+=2;
		}
		else
		{
			auto hi = HexTable[current >> 4];
			auto lo = HexTable[current & 0x0F];
			dest[writePtr] = hi;
			dest[writePtr + 1] = lo;
			dest[writePtr + 2] = currentCount - 1 + 'a';
			writePtr+=3;
		}
	};

	for (int i = 0; i < dataSize; i++)
	{
		uint8_t token = data[i];
		if (token != current)
		{
			Emit();
			current = token;
			currentCount = 1;
		}
		else
			currentCount++;

		if (currentCount >= 26)
		{
			Emit();
			currentCount = 0;
		}
	}

	Emit();

	return writePtr;
}

// Decompress the hexadecimal string from Compress() back into a byte array
int Decompress(const char* str, int strLen, uint8_t* dest)
{
	int writePtr = 0;
	int i = 0;

	auto Emit = [&](uint8_t val, int cnt)
	{
		for (int i = 0; i < cnt; i++)
		{
			dest[writePtr] = val;
			writePtr++;
		}
	};

	while (i < strLen)
	{
		auto hi = GetHexValue(str[i]);
		auto lo = GetHexValue(str[i+1]);
		uint8_t value = (hi << 4) | lo;
		int count = 1;
		int jump = 2;
		if (str[i + 2] >= 'a')
		{
			count = str[i + 2] - 'a' + 1;
			jump = 3;
		}

		Emit(value, count);
		i += jump;
	}

	return writePtr;
}
