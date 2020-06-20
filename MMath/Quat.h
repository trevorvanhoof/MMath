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
	DLL Quat QuatRotateX(const float radians);
	DLL Quat QuatRotateY(const float radians);
	DLL Quat QuatRotateZ(const float radians);
	DLL Quat QuatMul(const Quat lhs, const Quat rhs);
	DLL float QuatDot(const Quat a, const Quat b);
	DLL float QuatSqrMagnitude(const Quat q);
	DLL float QuatMagnitude(const Quat q);
	DLL Quat QuatNormalized(const Quat q, const Quat fallback);
	DLL Quat QuatInversed(const Quat q); // also known as conjugate
	DLL Quat QuatConjugated(const Quat q); // also known as inverse
	DLL Quat QuatSlerp(const Quat l, const Quat r, const float t);
	DLL Vec QuatVectorTransform(const Quat q, const __m128 v);
	DLL Vec QuatToEuler(const Quat q, const ERotateOrder order);
}
