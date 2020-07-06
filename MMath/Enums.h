/**
MMath - vector math library for 3D applications.
Released under the MIT License:

Copyright 2020 Trevor van Hoof

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/
#pragma once
#include "DLL.h"

extern "C"
{
	/*
	The values of this utility enum are important.
	Consider:
	X=0, Y=1, Z=2
	Every entry is then composed of three 2-bit integers concatenated in the order of writing.
	So XYZ is 0, 1, 2 is 0b00, 0b01, 0b10 in that order, which is 0b000110
	To retrieve values we must shift back to the front.
	firstAxisOfRotation = ERotateOrder::XYZ >> 4; // no mask necessary as these are the last bits in the stream
	secondAxisOfRotation = (ERotateOrder::XYZ >> 2) & 0b11; // shift 2 and mask to strip the other 2 significant bits
	thirdAxisOfRotation = ERotateOrder::XYZ & 0b11; // mask to keep least significant bits
	which results in firstAxisOfRotation == 0, secondAxisOfRotation  == 1, thirdAxisOfRotation == 3 for this particular example.
	*/
	enum class ERotateOrder
	{
		XYZ = 0b000110,
		YZX = 0b011000,
		ZXY = 0b100001,
		XZY = 0b001001,
		YXZ = 0b010010,
		ZYX = 0b100100
	};

	/*
	Axes as 3-bit numbers. The most significant bit is the sign bit, 1 is negative.
	The other 2 bits are the axis index, X = 0, Y = 1, Z = 2
	This way we can do stuff like
	static const __m128[] axes = {F32_UNIT_X,F32_UNIT_Y,F32_UNIT_Z, F32_ZERO, F32_NEG_UNIT_X,F32_NEG_UNIT_Y,F32_NEG_UNIT_Z};
	__m128 axis = axes[axis]; // axis is of type EAxis
	*/
	enum class EAxis
	{
		X = 0b000,
		Y = 0b001,
		Z = 0b010,
		NEG_X = 0b100,
		NEG_Y = 0b101,
		NEG_Z = 0b110
	};

	/*
	Matrix validation flags, pass in 1 bits to apply validation
	and receive 0 bits if validation passes.
	*/
	enum class Mat44ValidationFlags
	{
		ValidationOK = 0b00000, // what will be returned if all validations passed, when passed as 'flags' no validation will be performed
		Orthagonal = 0b00001, // when set, check for shearing, so all axes dot to 0 = OK
		Normalized = 0b00010, // when set, check for scaling, so all axes magnitude 1 = OK
		Uniform = 0b00100, // when set, check for uniform scaling, so all axes same length = OK
		NotFlipped = 0b01000, // when set, check for negative scaling, so all axes cross & dot > 0 = OK
		FourthRow = 0b10000, // when set, check for garbage in the last row, so col[0,1,2].w = 0 and col3.w = 1 = OK
	};
}

