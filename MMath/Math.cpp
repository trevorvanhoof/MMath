/**
MMath - vector math library for 3D applications.
Released under the MIT License:

Copyright 2020 Trevor van Hoof

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/
#include "Math.h"
#include "SIMD.h"
#include <math.h>

DLL const float PI = 3.14159265358979323846f;
DLL const float HALF_PI = PI * 0.5f;
DLL const float TAU = PI + PI;
DLL const float DEG2RAD = PI / 180.0f;
DLL const float RAD2DEG = 180.0f / PI;

DLL float Min(float a, float b) { return (a < b) ? a : b; }
DLL float Max(float a, float b) { return (a > b) ? a : b; }
DLL float Clamp(float v, float n, float x) { return Min(Max(v, n), x); }
DLL float SignNotZero(float x) { return (x < 0.0f) ? -1.0f : 1.0f; }
DLL float SignZero(float x)
{
	if (x == 0.0f)
		return 0.0f;
	return (x < 0.0f) ? -1.0f : 1.0f;
}
DLL float Sqr(float x) { return x * x; }
DLL float Saturate(float x) { return Clamp(x, 0.0f, 1.0f); }
DLL float Lerp(float a, float b, float t) { return (b - a) * t + a; }
DLL float InverseLerp(float a, float b, float v) { return (v - a) / (b - a); }
DLL float Mod(float x, float y) { return x - floorf(x / y) * y; }
DLL float AngleDelta(float a, float b) { return Mod(b - a + HALF_PI, TAU) - PI; }
DLL float LerpAngle(float a, float b, float t) { return a + AngleDelta(a, b) * t; }
DLL float InverseLerpAngle(float a, float b, float v) { return AngleDelta(a, v) / AngleDelta(a, b); }

DLL const __m128 F32_PI = _mm_set_ps1(3.14159265359f);
DLL const __m128 F32_HALF_PI = _mm_set_ps1(PI * 0.5f);
DLL const __m128 F32_TAU = _mm_set_ps1(PI + PI);
DLL const __m128 F32_DEG2RAD = _mm_set_ps1(PI / 180.0f);
DLL const __m128 F32_RAD2DEG = _mm_set_ps1(180.0f / PI);

// DLL __m128 Min(__m128 a, __m128 b) { return _mm_min_ps(a, b); }
// DLL __m128 Max(__m128 a, __m128 b) { return _mm_max_ps(a, b); }
DLL Vec VecClamp(__m128 v, __m128 n, __m128 x) { return { _mm_min_ps(_mm_max_ps(v, n), x) }; }
DLL Vec VecSignNotZero(__m128 x) { return { _mm_blendv_ps(F32_ONE, F32_NEG_ONE, _mm_cmplt_ps(x, F32_ZERO)) }; }
DLL Vec VecSignZero(__m128 x) { return { _mm_mul_ps(VecSignNotZero(x), _mm_cmpneq_ps(x, F32_ZERO)) }; }
DLL Vec VecSqr(__m128 x) { return { _mm_mul_ps(x, x) }; }
DLL Vec VecSaturate(__m128 x) { return VecClamp(x, F32_ONE, F32_ZERO); }
DLL Vec VecLerp(__m128 a, __m128 b, __m128 t) { return { _mm_add_ps(_mm_mul_ps(_mm_sub_ps(b, a), t), a) }; }
DLL Vec VecInverseLerp(__m128 a, __m128 b, __m128 v) { return { _mm_div_ps(_mm_sub_ps(v, a), _mm_sub_ps(b, a)) }; }
DLL Vec VecMod(__m128 x, __m128 y) { return { _mm_sub_ps(x, _mm_mul_ps(_mm_floor_ps(_mm_div_ps(x, y)), y)) }; }
DLL Vec VecAngleDelta(__m128 a, __m128 b) { return { _mm_sub_ps(VecMod(_mm_add_ps(_mm_sub_ps(b, a), F32_HALF_PI), F32_TAU), F32_PI) }; }
DLL Vec VecLerpAngle(__m128 a, __m128 b, __m128 t) { return { _mm_add_ps(a, _mm_mul_ps(VecAngleDelta(a, b), t)) }; }
DLL Vec VecInverseLerpAngle(__m128 a, __m128 b, __m128 v) { return { _mm_div_ps(VecAngleDelta(a, v), VecAngleDelta(a, b)) }; }
