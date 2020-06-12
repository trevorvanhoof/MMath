/**
MMath - vector math library for 3D applications.
Released under the MIT License:

Copyright 2020 Trevor van Hoof

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/
#include "SIMD.h"
#include "Math.h"
#include <math.h>

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

// internal
const __m128 F32_HALF = { 0.5f, 0.5f, 0.5f, 0.5f };
const __m128 F32_SIGN_MASK = _mm_castsi128_ps(_mm_set1_epi32((1 << 31)));
const __m128 F32_UNSIGEND_MASK = _mm_castsi128_ps(_mm_set1_epi32(~(1 << 31)));

const __m128 F32_FOUR_OVER_PI = { 4.0f / PI, 4.0f / PI, 4.0f / PI, 4.0f / PI };
const __m128i I32_ZERO = { 0, 0, 0, 0 };
const __m128i I32_ONE = { 1,1,1,1 };
const __m128i I32_MINUS_TWO = { -2,-2,-2,-2 }; // this is ~1
const __m128i I32_TWO = { 2,2,2,2 };
const __m128i I32_FOUR = { 4,4,4,4 };
const __m128 F32_DP1 = { -0.78515625f, -0.78515625f, -0.78515625f, -0.78515625f };
const __m128 F32_DP2 = { -2.4187564849853515625e-4f, -2.4187564849853515625e-4f, -2.4187564849853515625e-4f, -2.4187564849853515625e-4f };
const __m128 F32_DP3 = { -3.77489497744594108e-8f, -3.77489497744594108e-8f, -3.77489497744594108e-8f, -3.77489497744594108e-8f };
const __m128 F32_SIN_COEFF0 = { -1.9515295891E-4f, -1.9515295891E-4f, -1.9515295891E-4f, -1.9515295891E-4f };
const __m128 F32_SIN_COEFF1 = { 8.3321608736E-3f, 8.3321608736E-3f, 8.3321608736E-3f, 8.3321608736E-3f };
const __m128 F32_SIN_COEFF2 = { -1.6666654611E-1f, -1.6666654611E-1f, -1.6666654611E-1f, -1.6666654611E-1f };
const __m128 F32_COS_COEFF0 = { 2.443315711809948E-005f, 2.443315711809948E-005f, 2.443315711809948E-005f, 2.443315711809948E-005f };
const __m128 F32_COS_COEFF1 = { -1.388731625493765E-003f, -1.388731625493765E-003f,-1.388731625493765E-003f, -1.388731625493765E-003f };
const __m128 F32_COS_COEFF2 = { 4.166664568298827E-002f, 4.166664568298827E-002f, 4.166664568298827E-002f, 4.166664568298827E-002f };

__m128 _mm_abs_ps(__m128 v) { return _mm_and_ps(v, F32_UNSIGEND_MASK); }
__m128 _mm_sign_ps(__m128 v) { return _mm_and_ps(v, F32_SIGN_MASK); }
__m128 _mm_neg_ps(__m128 v) { return _mm_sub_ps(F32_ZERO, v); }

#if (_MSC_VER < 1920)
__forceinline __m128 _sin_ps(__m128 x, bool cosine = false)
{ // any x
	__m128 xmm1, xmm2 = _mm_setzero_ps(), xmm3, y, y2, z, sign_bit, poly_mask, tmp;
	__m128i emm0, emm2;

	__m128 sign_bit2;
	if (!cosine) sign_bit2 = x;
	// take the absolute value 
	x = _mm_abs_ps(x);
	// extract the sign bit (upper one) 
	if (!cosine) sign_bit2 = _mm_sign_ps(sign_bit2);

	// scale by 4/Pi 
	y = _mm_mul_ps(x, F32_FOUR_OVER_PI);

	// store the integer part of y in mm0 
	emm2 = _mm_cvttps_epi32(y);
	// j=(j+1) & (~1) (see the cephes sources) 
	emm2 = _mm_add_epi32(emm2, I32_ONE);
	emm2 = _mm_and_si128(emm2, I32_MINUS_TWO);
	y = _mm_cvtepi32_ps(emm2);
	if (cosine)
		emm2 = _mm_sub_epi32(emm2, I32_TWO);
	// get the swap sign flag 
	emm0 = cosine ? _mm_andnot_si128(emm2, I32_FOUR) : _mm_and_si128(emm2, I32_FOUR);
	emm0 = _mm_slli_epi32(emm0, 29);
	// get the polynom selection mask
	// there is one polynom for 0 <= x <= Pi / 4
	// and another one for Pi / 4 < x <= Pi / 2
	// Both branches will be computed.
	emm2 = _mm_and_si128(emm2, I32_TWO);
	emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());

	sign_bit = _mm_castsi128_ps(emm0);
	poly_mask = _mm_castsi128_ps(emm2);
	if (!cosine) sign_bit = _mm_xor_ps(sign_bit2, sign_bit);

	// The magic pass: "Extended precision modular arithmetic"
	// x = ((x - y * DP1) - y * DP2) - y * DP3;

	xmm1 = F32_DP1;
	xmm2 = F32_DP2;
	xmm3 = F32_DP3;
	xmm1 = _mm_mul_ps(y, xmm1);
	xmm2 = _mm_mul_ps(y, xmm2);
	xmm3 = _mm_mul_ps(y, xmm3);
	x = _mm_add_ps(x, xmm1);
	x = _mm_add_ps(x, xmm2);
	x = _mm_add_ps(x, xmm3);

	// Evaluate the first polynom  (0 <= x <= Pi/4) 
	y = F32_COS_COEFF0;
	z = _mm_mul_ps(x, x);

	y = _mm_mul_ps(y, z);
	y = _mm_add_ps(y, F32_COS_COEFF1);
	y = _mm_mul_ps(y, z);
	y = _mm_add_ps(y, F32_COS_COEFF2);
	y = _mm_mul_ps(y, z);
	y = _mm_mul_ps(y, z);
	tmp = _mm_mul_ps(z, F32_HALF);
	y = _mm_sub_ps(y, tmp);
	y = _mm_add_ps(y, F32_ONE);

	// Evaluate the second polynom  (Pi/4 <= x <= 0) 
	y2 = F32_SIN_COEFF0;
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_add_ps(y2, F32_SIN_COEFF1);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_add_ps(y2, F32_SIN_COEFF2);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_mul_ps(y2, x);
	y2 = _mm_add_ps(y2, x);

	// select the correct result from the two polynoms 
	xmm3 = poly_mask;
	y2 = _mm_and_ps(xmm3, y2); //, xmm3);
	y = _mm_andnot_ps(xmm3, y);
	y = _mm_add_ps(y, y2);
	// update the sign 
	y = _mm_xor_ps(y, sign_bit);

	return y;
}

__m128 _mm_sin_ps(__m128 x) { return _sin_ps(x); }
__m128 _mm_cos_ps(__m128 x) { return _sin_ps(x, true); }
#endif
