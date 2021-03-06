// Code is maybe under the zlib license (same as Irrlicht) if that's possible when using partly code from the web.
// Adapted to Irrlicht by Michael Zeilfelder but originally written by Bruce Dawson in this blog:
// http://randomascii.wordpress.com/2014/01/27/theres-only-four-billion-floatsso-test-them-all/

#define IRRLICHT_FAST_MATH 1
#include <irrMath.h>
#include <cstdio>
#include <cstdint>
#include <iostream>

// have to wrap the functions to receive and return float
float ceil32f(float x)
{
	return (float)irr::core::ceil32(x);
}

float orig_ceil(float x)
{
	return (float)ceil(x);
}

float floor32f(float x)
{
	return (float)irr::core::floor32(x);
}

float orig_floor(float x)
{
	return (float)floor(x);
}

float orig_round(float x)
{
	return (float)round(x);
}

float round32f(float x)
{
	return (float)irr::core::round32(x);
}

float orig_reciprocal_squareroot(float x)
{
	return 1.f / sqrtf(x);
}

float fastMath_reciprocal_squareroot(float x)
{
	return irr::core::reciprocal_squareroot(x);
}

float orig_reciprocal(float x)
{
	return 1.f / x;
}

float fastMath_reciprocal(float x)
{
	return irr::core::reciprocal(x);
}

union Float_t
{
	Float_t(float num = 0.0f) : f(num) {}
	// Portable extraction of components.
	bool Negative() const { return (i >> 31) != 0; }
	int32_t RawMantissa() const { return i & ((1 << 23) - 1); }
	int32_t RawExponent() const { return (i >> 23) & 0xFF; }

	int32_t i;
	float f;
#ifdef _DEBUG
	struct
	{   // Bitfields for exploration. Do not use in production code.
		uint32_t mantissa : 23;
		uint32_t exponent : 8;
		uint32_t sign : 1;
	} parts;
#endif
};

typedef float(*Transform)(float);

// Pass in a uint32_t range of float representations to test.
// start and stop are inclusive. Pass in 0, 0xFFFFFFFF to scan all
// floats. The floats are iterated through by incrementing
// their integer representation.
void ExhaustiveTest(uint32_t start, uint32_t stop, Transform TestFunc,
	Transform RefFunc, const char* desc)
{
	int bugs = 0;
	printf("Testing %s from %u to %u (inclusive).\n", desc, start, stop);
	// Use long long to let us loop over all positive integers.
	long long i = start;
	while (i <= stop)
	{
		Float_t input;
		input.i = (int32_t)i;
		Float_t testValue = TestFunc(input.f);
		Float_t refValue = RefFunc(input.f);
		// If the results don’t match then report an error.
		if (testValue.f != refValue.f &&
			// If both results are NaNs then we treat that as a match.
			(testValue.f == testValue.f || refValue.f == refValue.f))
		{
			printf("Input %.9g, expected %.9g, got %1.9g        \n",
				input.f, refValue.f, testValue.f);

			if ( ++bugs > 10 )
				return;
		}

		++i;
	}
}

int main()
{
	// This is the biggest number that can be represented in
	// both float and int32_t. It’s 2^31-128.
	Float_t maxfloatasint(2147483520.0f);
	const uint32_t signBit = 0x80000000;

	ExhaustiveTest(0, (uint32_t)maxfloatasint.i, floor32f, orig_floor,
		"floor");
	ExhaustiveTest(signBit, signBit | maxfloatasint.i, floor32f, orig_floor,
		"floorSignBit");
	ExhaustiveTest(0, (uint32_t)maxfloatasint.i, ceil32f, orig_ceil,
		"ceil");
	ExhaustiveTest(signBit, signBit | maxfloatasint.i, ceil32f, orig_ceil,
		"ceilSignBit");

	// Slightly wrong, mostly at height numbers and at 0.49999997
	ExhaustiveTest(0, (uint32_t)maxfloatasint.i, irr::core::round_, orig_round,
		"round");
	// Rounds other direction: Input -0.5, expected -1, got 0
	ExhaustiveTest(signBit, signBit | maxfloatasint.i, irr::core::round_, orig_round,
		"roundSignBit");

	ExhaustiveTest(0, (uint32_t)maxfloatasint.i, round32f, orig_round,
		"round32");
	ExhaustiveTest(signBit, signBit | maxfloatasint.i, fastMath_reciprocal_squareroot, orig_reciprocal_squareroot,
		"round32SignBit");

	ExhaustiveTest(0, (uint32_t)maxfloatasint.i, fastMath_reciprocal, orig_reciprocal,
			"reciprocal");
	ExhaustiveTest(signBit, signBit | maxfloatasint.i, fastMath_reciprocal, orig_reciprocal,
		"reciprocalSignBit");

	ExhaustiveTest(0, (uint32_t)maxfloatasint.i, fastMath_reciprocal_squareroot, orig_reciprocal_squareroot,
		"reciprocal_squareroot");
	ExhaustiveTest(signBit, signBit | maxfloatasint.i, fastMath_reciprocal_squareroot, orig_reciprocal_squareroot,
		"reciprocal_squarerootSignBit");

	std::cin.get();

	return 0;
}
