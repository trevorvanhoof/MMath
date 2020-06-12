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

	DLL float Min(float a, float b);
	DLL float Max(float a, float b);
	DLL float Clamp(float v, float n, float x);
	DLL float SignZero(float x);
	DLL float SignNotZero(float x);
	DLL float Sqr(float x); // simply multiply by self for easier inlining of stuff like (sqr(acos(x) / y))
	DLL float Saturate(float x); // Clamp01
	DLL float Lerp(float a, float b, float t); // Linear interpolate, implemented as: (b-a)*t+a, t remains unclamped so you can lerp beyond the endpoints.
	DLL float InverseLerp(float a, float b, float v); // solve for t in v = lerp(a, b, t)
	DLL float AngleDelta(float a, float b); // Get the angle begtween two angles considering the angle wraps around.
	DLL float Mod(float x, float y); // Mod is implemented using floor() instead of trunc() because I expect -1 % 2 = 1 (not -1), come on standard lib...
	DLL float LerpAngle(float a, float b, float t); // Linear interpolate, but understand the values wrap around at TAU, can interpolate up to a full circle but not more.
	DLL float InverseLerpAngle(float a, float b, float t); // solve for t in v = LerpAngle(a, b, t)

	DLL extern const __m128 F32_PI;
	DLL extern const __m128 F32_HALF_PI;
	DLL extern const __m128 F32_TAU;
	DLL extern const __m128 F32_DEG2RAD;
	DLL extern const __m128 F32_RAD2DEG;

	//DLL __m128 Min(__m128 a, __m128 b); // Already exists as _mm_min_ps
	//DLL __m128 Max(__m128 a, __m128 b); // Already exists as _mm_max_ps
	DLL Vec VecClamp(__m128 v, __m128 n, __m128 x);
	DLL Vec VecSignZero(__m128 x);
	DLL Vec VecSignNotZero(__m128 x);
	DLL Vec VecSqr(__m128 x); // simply multiply by self for easier inlining of stuff like (sqr(acos(x) / y))
	DLL Vec VecSaturate(__m128 x); // Clamp01
	DLL Vec VecLerp(__m128 a, __m128 b, __m128 t); // Linear interpolate, implemented as: (b-a)*t+a, t remains unclamped so you can lerp beyond the endpoints.
	DLL Vec VecInverseLerp(__m128 a, __m128 b, __m128 v); // solve for t in v = lerp(a, b, t)
	DLL Vec VecAngleDelta(__m128 a, __m128 b); // Get the angle begtween two angles considering the angle wraps around.
	DLL Vec VecMod(__m128 x, __m128 y); // Mod is implemented using floor() instead of trunc() because I expect -1 % 2 = 1 (not -1), come on standard lib...
	DLL Vec VecLerpAngle(__m128 a, __m128 b, __m128 t); // Linear interpolate, but understand the values wrap around at TAU, can interpolate up to a full circle but not more.
	DLL Vec VecInverseLerpAngle(__m128 a, __m128 b, __m128 t); // solve for t in v = LerpAngle(a, b, t)
}