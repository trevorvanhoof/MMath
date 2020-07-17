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

#include <immintrin.h>

// special swizzle
__forceinline __m128 _mm_swizzle_ps_0022(__m128 vec) { return _mm_moveldup_ps(vec); }
__forceinline __m128 _mm_swizzle_ps_1133(__m128 vec) { return _mm_movehdup_ps(vec); }

// special shuffle
__forceinline __m128 _mm_shuffle_ps_0101(__m128 vec1, __m128 vec2) { return _mm_movelh_ps(vec1, vec2); }
__forceinline __m128 _mm_shuffle_ps_2323(__m128 vec1, __m128 vec2) { return _mm_movehl_ps(vec2, vec1); }

// Missing vector instructions
__declspec(dllexport) __m128 _mm_abs_ps(__m128 v);
__declspec(dllexport) __m128 _mm_sign_ps(__m128 v);
__declspec(dllexport) __m128 _mm_neg_ps(__m128 v);

// I don't like using #defines so here's a bunch of swizzle functions.
// Sorry if it slows down compiles, so far it's worked fine!
__forceinline __m128 _mm_swizzle_ps_0033(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b11110000)); }
__forceinline __m128 _mm_swizzle_ps_0202(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b10001000)); }
__forceinline __m128 _mm_swizzle_ps_0203(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b11001000)); }
__forceinline __m128 _mm_swizzle_ps_0213(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b11011000)); }
__forceinline __m128 _mm_swizzle_ps_0223(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b11101000)); }
__forceinline __m128 _mm_swizzle_ps_0303(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b11001100)); }

__forceinline __m128 _mm_swizzle_ps_1000(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b00000001)); }
__forceinline __m128 _mm_swizzle_ps_1032(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b10110001)); }
__forceinline __m128 _mm_swizzle_ps_1122(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b10100101)); }
__forceinline __m128 _mm_swizzle_ps_1203(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b11001001)); }
__forceinline __m128 _mm_swizzle_ps_1313(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b11011101)); }

__forceinline __m128 _mm_swizzle_ps_2020(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b00100010)); }
__forceinline __m128 _mm_swizzle_ps_2121(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b01100110)); }
__forceinline __m128 _mm_swizzle_ps_2211(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b01011010)); }
__forceinline __m128 _mm_swizzle_ps_2301(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b01001110)); }

__forceinline __m128 _mm_swizzle_ps_3030(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b00110011)); }
__forceinline __m128 _mm_swizzle_ps_3131(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b01110111)); }
__forceinline __m128 _mm_swizzle_ps_3210(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b00011011)); }
__forceinline __m128 _mm_swizzle_ps_3300(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b00001111)); }

__forceinline __m128 _mm_swizzle_ps_0(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b00000000)); }
__forceinline __m128 _mm_swizzle_ps_1(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b01010101)); }
__forceinline __m128 _mm_swizzle_ps_2(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b10101010)); }
__forceinline __m128 _mm_swizzle_ps_3(__m128 v) { return  _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), 0b11111111)); }

__forceinline __m128 _mm_shuffle_ps_0202(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b10001000); }
__forceinline __m128 _mm_shuffle_ps_0203(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b11001000); }
__forceinline __m128 _mm_shuffle_ps_0213(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b11011000); }
__forceinline __m128 _mm_shuffle_ps_0223(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b11101000); }
__forceinline __m128 _mm_shuffle_ps_0303(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b11001100); }

__forceinline __m128 _mm_shuffle_ps_1032(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b10110001); }
__forceinline __m128 _mm_shuffle_ps_1122(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b10100101); }
__forceinline __m128 _mm_shuffle_ps_1313(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b11011101); }

__forceinline __m128 _mm_shuffle_ps_2020(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b00100010); }
__forceinline __m128 _mm_shuffle_ps_2121(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b01100110); }
__forceinline __m128 _mm_shuffle_ps_2301(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b01001110); }

__forceinline __m128 _mm_shuffle_ps_3030(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b00110011); }
__forceinline __m128 _mm_shuffle_ps_3131(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b01110111); }
__forceinline __m128 _mm_shuffle_ps_3300(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b00001111); }

__forceinline __m128 _mm_shuffle_ps_0(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b00000000); }
__forceinline __m128 _mm_shuffle_ps_1(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b01010101); }
__forceinline __m128 _mm_shuffle_ps_2(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b10101010); }
__forceinline __m128 _mm_shuffle_ps_3(__m128 a, __m128 b) { return  _mm_shuffle_ps(a, b, 0b11111111); }

#if (_MSC_VER < 1920)
// If you get linker errors for duplicate implementations, simply turn these off as Visual Studio 2019 and the latest Windows 10 SDK has these functions available!
__m128 _mm_sin_ps(__m128 x);
__m128 _mm_cos_ps(__m128 x);
#endif

extern const __m128 F32_ZERO;
extern const __m128 F32_ONE;
extern const __m128 F32_NEG_ONE;
extern const __m128 F32_UNIT_X;
extern const __m128 F32_UNIT_Y;
extern const __m128 F32_UNIT_Z;
extern const __m128 F32_UNIT_W;
extern const __m128 F32_UNIT_NEG_X;
extern const __m128 F32_UNIT_NEG_Y;
extern const __m128 F32_UNIT_NEG_Z;
extern const __m128 F32_UNIT_NEG_W;

// This factors are reused in several cpp files.
extern const __m128 F32_SIGNFLIP_0101; // in the naming 1 means _do_ sign flip => (1, -1, 1, -1)
extern const __m128 F32_SIGNFLIP_0110;
extern const __m128 F32_SIGNFLIP_0011;

extern const __m128 F32_SIGNFLIP_VEC3_100; // multiply with this to get the sign flip and w = 0
extern const __m128 F32_SIGNFLIP_VEC3_010;
extern const __m128 F32_SIGNFLIP_VEC3_001;

// For some reason dllexport didn't work so I just add this flag for third party users.
#ifdef IMPL_CONSTS

// exposed
const __m128 F32_ZERO = { 0.0f, 0.0f, 0.0f, 0.0f };
const __m128 F32_ONE = { 1.0f, 1.0f, 1.0f, 1.0f };
const __m128 F32_NEG_ONE = _mm_set_ps1(-1.0f);
const __m128 F32_UNIT_X = { 1.0f, 0.0f, 0.0f, 0.0f };
const __m128 F32_UNIT_Y = { 0.0f, 1.0f, 0.0f, 0.0f };
const __m128 F32_UNIT_Z = { 0.0f, 0.0f, 1.0f, 0.0f };
const __m128 F32_UNIT_W = { 0.0f, 0.0f, 0.0f, 1.0f };
const __m128 F32_UNIT_NEG_X = { -1.0f, 0.0f, 0.0f, 0.0f };
const __m128 F32_UNIT_NEG_Y = { 0.0f, -1.0f, 0.0f, 0.0f };
const __m128 F32_UNIT_NEG_Z = { 0.0f, 0.0f, -1.0f, 0.0f };
const __m128 F32_UNIT_NEG_W = { 0.0f, 0.0f, 0.0f, -1.0f };
const __m128 F32_SIGNFLIP_0101 = { 1.0f, -1.0f, 1.0f, -1.0f };
const __m128 F32_SIGNFLIP_0011 = { 1.0f, 1.0f, -1.0f, -1.0f };
const __m128 F32_SIGNFLIP_0110 = { 1.0f, -1.0f, -1.0f, 1.0f };

// sign flip and zero out w
const __m128 F32_SIGNFLIP_VEC3_100 = { -1.0f, 1.0f, 1.0f, 0.0f };
const __m128 F32_SIGNFLIP_VEC3_010 = { 1.0f, -1.0f, 1.0f, 0.0f };
const __m128 F32_SIGNFLIP_VEC3_001 = { 1.0f, 1.0f, -1.0f, 0.0f };

#endif
