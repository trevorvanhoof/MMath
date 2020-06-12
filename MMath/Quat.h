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

#include <xmmintrin.h>
#include "Enums.h"
#include "Vector.h"

extern "C"
{
	__declspec(align(16)) struct Quat
	{
		union
		{
			__m128 q;
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
			float s[4];
		};
	};
}

extern const Quat QUAT_IDENTITY;

extern "C"
{
	DLL Quat QuatIdentity();
	DLL Quat QuatRotateX(float radians);
	DLL Quat QuatRotateY(float radians);
	DLL Quat QuatRotateZ(float radians);
	DLL Quat QuatMul(Quat lhs, Quat rhs);
	DLL float QuatDot(Quat a, Quat b);
	DLL float QuatSqrMagnitude(Quat q);
	DLL float QuatMagnitude(Quat q);
	DLL Quat QuatNormalized(Quat q, Quat fallback);
	DLL Quat QuatInversed(Quat q); // also known as conjugate
	DLL Quat QuatConjugated(Quat q); // also known as inverse
	DLL Quat QuatSlerp(Quat l, Quat r, float t);
	DLL Vec QuatVectorTransform(Quat q, __m128 v);
	DLL Vec QuatToEuler(Quat q, ERotateOrder order);
}
