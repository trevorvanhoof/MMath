/**
MMath - vector math library for 3D applications.
Released under the MIT License:

Copyright 2020 Trevor van Hoof

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/
#pragma once

// Math contains utilities used throughout the rest of the DLL
// We mark them for export in case some of them are useful, 
// things like InverseLerpAngle can be pretty uncommon to have natively available.
// 
// It is recommended to use constants and functions belonging to the host language 
// where available as going across a DLL boundary for too granular 
// functions is not going to save time.

#include "DLL.h"
#include <xmmintrin.h>
#include "Vector.h"

extern "C"
{
	DLL extern const float PI;
	DLL extern const float HALF_PI;
	DLL extern const float TAU;
	DLL extern const float DEG2RAD;
	DLL extern const float RAD2DEG;

	DLL float Min(const float a, const float b);
	DLL float Max(const float a, const float b);
	DLL float Clamp(const float v, const float n, const float x);
	DLL float SignZero(const float x);
	DLL float SignNotZero(const float x);
	DLL float Sqr(const float x); // simply multiply by self for easier inlining of stuff like (sqr(acos(x) / y))
	DLL float Saturate(const float x); // Clamp01
	DLL float Lerp(const float a, const float b, const float t); // Linear interpolate, implemented as: (b-a)*t+a, t remains unclamped so you can lerp beyond the endpoints.
	DLL float InverseLerp(const float a, const float b, const float v); // solve for t in v = lerp(a, b, t)
	DLL float AngleDelta(const float a, const float b); // Get the angle begtween two angles considering the angle wraps around.
	DLL float Mod(const float x, const float y); // Mod is implemented using floor() instead of trunc() because I expect -1 % 2 = 1 (not -1), come on standard lib...
	DLL float LerpAngle(const float a, const float b, const float t); // Linear interpolate, but understand the values wrap around at TAU, can interpolate up to a full circle but not more.
	DLL float InverseLerpAngle(const float a, const float b, const float t); // solve for t in v = LerpAngle(a, b, t)

	DLL extern const __m128 F32_PI;
	DLL extern const __m128 F32_HALF_PI;
	DLL extern const __m128 F32_TAU;
	DLL extern const __m128 F32_DEG2RAD;
	DLL extern const __m128 F32_RAD2DEG;

	//DLL Vec Min(const __m128 a, const __m128 b); // Already exists as _mm_min_ps
	//DLL Vec Max(const __m128 a, const __m128 b); // Already exists as _mm_max_ps
	DLL Vec VecClamp(const __m128 v, const __m128 n, const __m128 x);
	DLL Vec VecSignZero(const __m128 x);
	DLL Vec VecSignNotZero(const __m128 x);
	DLL Vec VecSqr(const __m128 x); // simply multiply by self for easier inlining of stuff like (sqr(acos(x) / y))
	DLL Vec VecSaturate(const __m128 x); // Clamp01
	DLL Vec VecLerp(const __m128 a, const __m128 b, const __m128 t); // Linear interpolate, implemented as: (b-a)*t+a, t remains unclamped so you can lerp beyond the endpoints.
	DLL Vec VecInverseLerp(const __m128 a, const __m128 b, const __m128 v); // solve for t in v = lerp(a, b, t)
	DLL Vec VecAngleDelta(const __m128 a, const __m128 b); // Get the angle begtween two angles considering the angle wraps around.
	DLL Vec VecMod(const __m128 x, const __m128 y); // Mod is implemented using floor() instead of trunc() because I expect -1 % 2 = 1 (not -1), come on standard lib...
	DLL Vec VecLerpAngle(const __m128 a, const __m128 b, const __m128 t); // Linear interpolate, but understand the values wrap around at TAU, can interpolate up to a full circle but not more.
	DLL Vec VecInverseLerpAngle(const __m128 a, const __m128 b, const __m128 t); // solve for t in v = LerpAngle(a, b, t)
}

#ifdef IMPL_CONSTS
DLL const float PI = 3.14159265358979323846f;
DLL const float HALF_PI = PI * 0.5f;
DLL const float TAU = PI + PI;
DLL const float DEG2RAD = PI / 180.0f;
DLL const float RAD2DEG = 180.0f / PI;

DLL const __m128 F32_PI = _mm_set_ps1(3.14159265359f);
DLL const __m128 F32_HALF_PI = _mm_set_ps1(PI * 0.5f);
DLL const __m128 F32_TAU = _mm_set_ps1(PI + PI);
DLL const __m128 F32_DEG2RAD = _mm_set_ps1(PI / 180.0f);
DLL const __m128 F32_RAD2DEG = _mm_set_ps1(180.0f / PI);
#endif
