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

extern "C"
{
	// We can't reutrn __m128 accross DLL boundares, so we're casting it to our own Vec object
	// which hopefully compiles to no unnecessary overhead.
	__declspec(align(16)) struct Vec
	{
		union
		{
			__m128 s;
			struct
			{
				float x, y, z, w;
			};
		};
		operator __m128() { return s; }
	};

	DLL float Vec4Dot(const __m128 a, const __m128 b); // Cos(theta) * Magnitude(a) * Magnitude(b) = Dot(a, b), theta = ACos(Dot(a, b)) if a, b ar normalized.
	DLL float Vec4SqrMagnitude(const __m128 v); // Get the squared length of the vector, avoid Magnitude() and sqrt() calls whenever possible.
	DLL float Vec4Magnitude(const __m128 v); // Get the length of the vector.
	DLL Vec Vec4Normalized(const __m128 v, const __m128 fallback); // Divide the vector by it's own length. Returns fallback if input is ZERO (to avoid returning NaN or infinity).
	DLL Vec Vec4NormalizedUnsafe(const __m128 v); // This version has no fallback and will return garbage when used on zero-length vectors. Breaks in debug mode to help you spot when that happens.
	DLL Vec Vec4Perpendicular(const __m128 v); // Find a vector at 90 degrees from this vector in any plane this vector lies in

	// As a rule of thumb all Vec3 functions return with w = 0 and assume the input w is garbage
	DLL float Vec3Dot(const __m128 a, const __m128 b);
	DLL Vec Vec3Cross(const __m128 a, const __m128 b);
	DLL float Vec3SqrMagnitude(const __m128 v);
	DLL float Vec3Magnitude(const __m128 v);
	DLL Vec Vec3Normalized(const __m128 v, const __m128 fallback);
	DLL Vec Vec3NormalizedUnsafe(const __m128 v);
	DLL Vec Vec3Perpendicular(const __m128 v); // Find a vector at 90 degrees from this vector in any plane this vector lies in

	// As a rule of thumb all Vec2 functions return with zw = 0 and assume the input zw is garbage
	DLL float Vec2Dot(const __m128 a, const __m128 b);
	DLL float Vec2Cross(const __m128 a, const __m128 b);
	DLL float Vec2SqrMagnitude(const __m128 v);
	DLL float Vec2Magnitude(const __m128 v);
	DLL Vec Vec2Normalized(const __m128 v, const __m128 fallback);
	DLL Vec Vec2NormalizedUnsafe(const __m128 v);
	DLL Vec Vec2Perpendicular(const __m128 v); // Find a vector at 90 degrees from this vector in any plane this vector lies in
}

extern const __m128 F32_VEC2_MASK;
extern const __m128 F32_VEC3_MASK;
