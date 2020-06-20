/**
MMath - vector math library for 3D applications.
Released under the MIT License:

Copyright 2020 Trevor van Hoof

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/
#include "Vector.h"

#include "Math.h"
#include "SIMD.h"
#include <math.h>

const __m128 F32_VEC2_MASK = { 1.0f, 1.0f, 0.0f, 0.0f };
const __m128 F32_VEC3_MASK = { 1.0f, 1.0f, 1.0f, 0.0f };

float VecMaskDot(__m128 a, __m128 b, __m128 mask)
{
	return Vec4Dot(_mm_mul_ps(a, mask), b);
}
float VecMaskSqrMagnitude(__m128 v, __m128 mask)
{
	v = _mm_mul_ps(v, mask);
	return Vec4Dot(v, v);
}
float VecMaskMagnitude(__m128 v, __m128 mask)
{
	v = _mm_mul_ps(v, mask);
	return sqrtf(Vec4Dot(v, v));
}
Vec VecMaskNormalized(__m128 v, __m128 mask, __m128 fallback)
{
	v = _mm_mul_ps(v, mask);
	__m128 a = _mm_mul_ps(v, v);
	a = _mm_hadd_ps(a, a);
	a = _mm_hadd_ps(a, a);
#if 0
	if (a.m128_f32[0] == 0.0f) 
		return _mm_mul_ps(fallback, mask);
	return _mm_mul_ps(v, _mm_rsqrt_ps(a));
#endif
#if 1
	// TODO: There must be a better way
	__m128 isZero = _mm_cmpeq_ps(a, F32_ZERO);
	// return _mm_mul_ps(_mm_blendv_ps(v, fallback, isZero), _mm_blendv_ps(_mm_rsqrt_ps(a), mask, isZero));
	return { _mm_blendv_ps(_mm_mul_ps(v, _mm_rsqrt_ps(a)), _mm_mul_ps(fallback, mask), isZero) };
#endif
}
Vec VecMaskNormalized(__m128 v, __m128 mask)
{
	v = _mm_mul_ps(v, mask);
	__m128 a = _mm_mul_ps(v, v);
	a = _mm_hadd_ps(a, a);
	a = _mm_hadd_ps(a, a);
#if _DEBUG
	if (a.m128_f32[0] == 0.0f)
		__debugbreak();
#endif
	return { _mm_mul_ps(v, _mm_rsqrt_ps(a)) };
}

extern "C"
{
	DLL float Vec4Dot(const __m128 c, const __m128 b)
	{
		__m128 a = c;
		a = _mm_mul_ps(a, b);
		a = _mm_hadd_ps(a, a);
		a = _mm_hadd_ps(a, a);
		return a.m128_f32[0];
	}

	DLL float Vec4SqrMagnitude(const __m128 v)
	{
		return Vec4Dot(v, v);
	}

	DLL float Vec4Magnitude(const __m128 v)
	{
		return sqrtf(Vec4SqrMagnitude(v));
	}

	DLL Vec Vec4Normalized(const __m128 v, const __m128 fallback)
	{
		__m128 a = _mm_mul_ps(v, v);
		a = _mm_hadd_ps(a, a);
		a = _mm_hadd_ps(a, a);
#if 0
		if (a.m128_f32[0] == 0.0f)
			return fallback;
		return { _mm_mul_ps(v, _mm_rsqrt_ps(a)) };
#endif
#if 1
		// TODO: There must be a better way
		__m128 isZero = _mm_cmpeq_ps(a, F32_ZERO);
		return { _mm_blendv_ps(_mm_mul_ps(v, _mm_rsqrt_ps(a)), fallback, isZero) };
#endif
	}

	DLL Vec Vec4NormalizedUnsafe(const __m128 v)
	{
		__m128 a = _mm_mul_ps(v, v);
		a = _mm_hadd_ps(a, a);
		a = _mm_hadd_ps(a, a);
#if _DEBUG
		if (a.m128_f32[0] == 0.0f)
			__debugbreak();
#endif
		return{ _mm_mul_ps(v, _mm_rsqrt_ps(a)) };
	}

	DLL Vec Vec4Perpendicular(const __m128 v)
	{
		// Find a vector at 90 degrees from this vector in any plane this vector lies in, implented from this answer as reference:
		// https://math.stackexchange.com/questions/133177/finding-a-unit-vector-perpendicular-to-another-vector/413235#413235
		__m128 y = _mm_setzero_ps();
		int m = 0;
		// TODO: Try and unroll this to see perf difference
		while (v.m128_f32[m] == 0.0f && m != 4) { ++m; }
		if (m == 4)
		{
			__debugbreak(); // taking perpendicular vector to zero, returning arbitrary fallback UNIT_X
			return { F32_UNIT_X };
		}
		int n = (m + 1) % 4;
		y.m128_f32[n] = v.m128_f32[m];
		y.m128_f32[m] = -v.m128_f32[n];
		return Vec4NormalizedUnsafe(y);
	}
	
	DLL float Vec3Dot(const __m128 a, const __m128 b) { return VecMaskDot(a, b, F32_VEC3_MASK); }
	DLL float Vec3SqrMagnitude(const __m128 v) { return VecMaskSqrMagnitude(v, F32_VEC3_MASK); }
	DLL float Vec3Magnitude(const __m128 v) { return VecMaskMagnitude(v, F32_VEC3_MASK); }
	DLL Vec Vec3Normalized(const __m128 v, const __m128 fallback) { return VecMaskNormalized(v, F32_VEC3_MASK, fallback); }
	DLL Vec Vec3NormalizedUnsafe(const __m128 v) { return VecMaskNormalized(v, F32_VEC3_MASK); }
	
	DLL Vec Vec3Cross(const __m128 a, const __m128 b)
	{
		__m128 a2 = _mm_swizzle_ps_1203(a);
		__m128 b2 = _mm_swizzle_ps_1203(b);
		__m128 v = _mm_swizzle_ps_1203(_mm_sub_ps(_mm_mul_ps(a, b2), _mm_mul_ps(b, a2)));
		return { _mm_mul_ps(v, F32_VEC3_MASK) };
	}

	DLL Vec Vec3Perpendicular(const __m128 v)
	{
		// Find a vector at 90 degrees from this vector in any plane this vector lies in, implented from this answer as reference:
		// https://math.stackexchange.com/questions/133177/finding-a-unit-vector-perpendicular-to-another-vector/413235#413235
		__m128 y = _mm_setzero_ps();
		int m = 0;
		// TODO: Try and unroll this to see perf difference
		while (v.m128_f32[m] == 0.0f && m != 4) { ++m; }
		if (m == 3)
		{
			__debugbreak(); // taking perpendicular vector to zero, returning arbitrary fallback UNIT_X
			return { F32_UNIT_X };
		}
		int n = (m + 1) % 3;
		y.m128_f32[n] = v.m128_f32[m];
		y.m128_f32[m] = -v.m128_f32[n];
		return Vec4NormalizedUnsafe(y);
	}

	DLL float Vec2Dot(const __m128 a, const __m128 b) { return VecMaskDot(a, b, F32_VEC2_MASK); }
	DLL float Vec2SqrMagnitude(const __m128 v) { return VecMaskSqrMagnitude(v, F32_VEC2_MASK); }
	DLL float Vec2Magnitude(const __m128 v) { return VecMaskMagnitude(v, F32_VEC2_MASK); }
	DLL Vec Vec2Normalized(const __m128 v, const __m128 fallback) { return VecMaskNormalized(v, F32_VEC2_MASK, fallback); }
	DLL Vec Vec2NormalizedUnsafe(const __m128 v) { return VecMaskNormalized(v, F32_VEC2_MASK); }

	DLL float Vec2Cross(const __m128 a, const __m128 b)
	{
#if 0
		return a.m128_f32[0] * b.m128_f32[1] - a.m128_f32[1] * b.m128_f32[0];
#else
		__m128 v = _mm_mul_ps(a, _mm_swizzle_ps_1000(b));
		return v.m128_f32[0] - v.m128_f32[1];
#endif
	}

	DLL Vec Vec2Perpendicular(const __m128 v)
	{
		// Find a vector at 90 degrees from this vector in any plane this vector lies in, implented from this answer as reference:
		// https://math.stackexchange.com/questions/133177/finding-a-unit-vector-perpendicular-to-another-vector/413235#413235
		__m128 y = _mm_setzero_ps();
		int m = 0;
		// TODO: Try and unroll this to see perf difference
		while (v.m128_f32[m] == 0.0f && m != 2) { ++m; }
		if (m == 4)
		{
			__debugbreak(); // taking perpendicular vector to zero, returning arbitrary fallback UNIT_X
			return { F32_UNIT_X };
		}
		int n = (m + 1) % 2;
		y.m128_f32[n] = v.m128_f32[m];
		y.m128_f32[m] = -v.m128_f32[n];
		return Vec4NormalizedUnsafe(y);
	}
}

// This section is to export commonly used component-wise SIMD functions for vectors
// C++ users should not be using these so they're not incuded in the header file.
#include "SIMD.h"
#include <windows.h>

extern "C"
{
	DLL Vec VecAdd(const __m128 lhs, const __m128  rhs) { return { _mm_add_ps(lhs, rhs) }; }
	DLL Vec VecSub(const __m128 lhs, const __m128 rhs) { return { _mm_sub_ps(lhs, rhs) }; }
	DLL Vec VecMul(const __m128 lhs, const __m128 rhs) { return { _mm_mul_ps(lhs, rhs) }; }
	DLL Vec VecDiv(const __m128 lhs, const __m128 rhs) { return { _mm_div_ps(lhs, rhs) }; }
	DLL Vec VecMin(const __m128 lhs, const __m128 rhs) { return { _mm_min_ps(lhs, rhs) }; }
	DLL Vec VecMax(const __m128 lhs, const __m128 rhs) { return { _mm_max_ps(lhs, rhs) }; }
	DLL Vec VecAbs(const __m128 lhs) { return { _mm_abs_ps(lhs) }; }
	DLL Vec VecSign(const __m128 lhs) { return { _mm_sign_ps(lhs) }; }
	DLL Vec VecNegate(const __m128 lhs) { return { _mm_neg_ps(lhs) }; }
	DLL Vec VecSin(const __m128 lhs) { return { _mm_sin_ps(lhs) }; }
	DLL Vec VecCos(const __m128 lhs) { return { _mm_cos_ps(lhs) }; }
	DLL Vec VecFloor(const __m128 lhs) { return { _mm_floor_ps(lhs) }; }
	DLL Vec VecCeil(const __m128 lhs) { return { _mm_ceil_ps(lhs) }; }
	DLL Vec VecRound(const __m128 lhs) { return { _mm_round_ps(lhs, _MM_FROUND_NINT) }; }
}
