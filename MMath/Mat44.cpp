/**
MMath - vector math library for 3D applications.
Released under the MIT License:

Copyright 2020 Trevor van Hoof

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/
#include "Mat44.h"
#include "SIMD.h"
#include "Math.h"
#include "Vector.h"
#include "Enums.h"
#include "Friends.h"
#include <math.h>

static const __m128 F32_SIGNFLIP_1110 = { -1.0f, -1.0f, -1.0f, 1.0f };
#pragma region(eric_matrix_inversion)
#if 1
// https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html
static const float SMALL_NUMBER = 1.e-8f;

// Requires this matrix to be transform matrix, NoScale version requires this matrix be of scale 1
inline Mat44 GetTransformInverseNoScale(const Mat44& inM)
{
	Mat44 r;

	// transpose 3x3, we know m03 = m13 = m23 = 0
	__m128 t0 = _mm_shuffle_ps_0101(inM.cols[0], inM.cols[1]); // 00, 01, 10, 11
	__m128 t1 = _mm_shuffle_ps_2323(inM.cols[0], inM.cols[1]); // 02, 03, 12, 13
	r.cols[0] = _mm_shuffle_ps_0203(t0, inM.cols[2]); // 00, 10, 20, 23(=0)
	r.cols[1] = _mm_shuffle_ps_1313(t0, inM.cols[2]); // 01, 11, 21, 23(=0)
	r.cols[2] = _mm_shuffle_ps_0223(t1, inM.cols[2]); // 02, 12, 22, 23(=0)

	// last line
	r.cols[3] = _mm_mul_ps(r.cols[0], _mm_swizzle_ps_0(inM.cols[3]));
	r.cols[3] = _mm_add_ps(r.cols[3], _mm_mul_ps(r.cols[1], _mm_swizzle_ps_1(inM.cols[3])));
	r.cols[3] = _mm_add_ps(r.cols[3], _mm_mul_ps(r.cols[2], _mm_swizzle_ps_2(inM.cols[3])));
	r.cols[3] = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), r.cols[3]);

	return r;
}

// Requires this matrix to be transform matrix
inline Mat44 GetTransformInverse(const Mat44& inM)
{
	Mat44 r;

	// transpose 3x3, we know m03 = m13 = m23 = 0
	__m128 t0 = _mm_shuffle_ps_0101(inM.cols[0], inM.cols[1]); // 00, 01, 10, 11
	__m128 t1 = _mm_shuffle_ps_2323(inM.cols[0], inM.cols[1]); // 02, 03, 12, 13
	r.cols[0] = _mm_shuffle_ps_0203(t0, inM.cols[2]); // 00, 10, 20, 23(=0)
	r.cols[1] = _mm_shuffle_ps_1313(t0, inM.cols[2]); // 01, 11, 21, 23(=0)
	r.cols[2] = _mm_shuffle_ps_0223(t1, inM.cols[2]); // 02, 12, 22, 23(=0)

	// (SizeSqr(cols[0]), SizeSqr(cols[1]), SizeSqr(cols[2]), 0)
	__m128 sizeSqr;
	sizeSqr = _mm_mul_ps(r.cols[0], r.cols[0]);
	sizeSqr = _mm_add_ps(sizeSqr, _mm_mul_ps(r.cols[1], r.cols[1]));
	sizeSqr = _mm_add_ps(sizeSqr, _mm_mul_ps(r.cols[2], r.cols[2]));

	// optional test to avoid divide by 0
	__m128 one = _mm_set_ps1(1.f);
	// for each component, if(sizeSqr < SMALL_NUMBER) sizeSqr = 1;
	__m128 rSizeSqr = _mm_blendv_ps(
		_mm_div_ps(one, sizeSqr),
		one,
		_mm_cmplt_ps(sizeSqr, _mm_set_ps1(SMALL_NUMBER))
	);

	r.cols[0] = _mm_mul_ps(r.cols[0], rSizeSqr);
	r.cols[1] = _mm_mul_ps(r.cols[1], rSizeSqr);
	r.cols[2] = _mm_mul_ps(r.cols[2], rSizeSqr);

	// last line
	r.cols[3] = _mm_mul_ps(r.cols[0], _mm_swizzle_ps_0(inM.cols[3]));
	r.cols[3] = _mm_add_ps(r.cols[3], _mm_mul_ps(r.cols[1], _mm_swizzle_ps_1(inM.cols[3])));
	r.cols[3] = _mm_add_ps(r.cols[3], _mm_mul_ps(r.cols[2], _mm_swizzle_ps_2(inM.cols[3])));
	r.cols[3] = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), r.cols[3]);

	return r;
}
#if 0 // 1 for row major matrix, 0 for column major matrix
// we use __m128 to represent 2x2 matrix as A = | A0  A1 |
//                                              | A2  A3 |
// 2x2 row major Matrix multiply A*B
__forceinline __m128 Mat2Mul(__m128 vec1, __m128 vec2)
{
	return
		_mm_add_ps(_mm_mul_ps(vec1, _mm_swizzle_ps_0303(vec2)),
			_mm_mul_ps(_mm_swizzle_ps_1032(vec1), _mm_swizzle_ps_2121(vec2)));
}
// 2x2 row major Matrix adjugate multiply (A#)*B
__forceinline __m128 Mat2AdjMul(__m128 vec1, __m128 vec2)
{
	return
		_mm_sub_ps(_mm_mul_ps(_mm_swizzle_ps_3300(vec1), vec2),
			_mm_mul_ps(_mm_swizzle_ps_1122(vec1), _mm_swizzle_ps_2301(vec2)));

}
// 2x2 row major Matrix multiply adjugate A*(B#)
__forceinline __m128 Mat2MulAdj(__m128 vec1, __m128 vec2)
{
	return
		_mm_sub_ps(_mm_mul_ps(vec1, _mm_swizzle_ps_3030(vec2)),
			_mm_mul_ps(_mm_swizzle_ps_1032(vec1), _mm_swizzle_ps_2121(vec2)));
}

// Inverse function is the same no matter column major or row major
// this version treats it as row major
inline Mat44 GetInverse(const Mat44& inM)
{
	// use block matrix method
	// A is a matrix, then i(A) or iA means inverse of A, A# (or A_ in code) means adjugate of A, |A| (or detA in code) is determinant, tr(A) is trace

	// sub matrices
	__m128 A = _mm_shuffle_ps_0101(inM.cols[0], inM.cols[1]);
	__m128 B = _mm_shuffle_ps_2323(inM.cols[0], inM.cols[1]);
	__m128 C = _mm_shuffle_ps_0101(inM.cols[2], inM.cols[3]);
	__m128 D = _mm_shuffle_ps_2323(inM.cols[2], inM.cols[3]);

#if 0
	__m128 detA = _mm_set1_ps(inM.m[0][0] * inM.m[1][1] - inM.m[0][1] * inM.m[1][0]);
	__m128 detB = _mm_set1_ps(inM.m[0][2] * inM.m[1][3] - inM.m[0][3] * inM.m[1][2]);
	__m128 detC = _mm_set1_ps(inM.m[2][0] * inM.m[3][1] - inM.m[2][1] * inM.m[3][0]);
	__m128 detD = _mm_set1_ps(inM.m[2][2] * inM.m[3][3] - inM.m[2][3] * inM.m[3][2]);
#else
	// determinant as (|A| |B| |C| |D|)
	__m128 detSub = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps_0202(inM.cols[0], inM.cols[2]), _mm_shuffle_ps_1313(inM.cols[1], inM.cols[3])),
		_mm_mul_ps(_mm_shuffle_ps_1313(inM.cols[0], inM.cols[2]), _mm_shuffle_ps_0202(inM.cols[1], inM.cols[3]))
	);
	__m128 detA = _mm_swizzle_ps_0(detSub);
	__m128 detB = _mm_swizzle_ps_1(detSub);
	__m128 detC = _mm_swizzle_ps_2(detSub);
	__m128 detD = _mm_swizzle_ps_3(detSub);
#endif

	// let iM = 1/|M| * | X  Y |
	//                  | Z  W |

	// D#C
	__m128 D_C = Mat2AdjMul(D, C);
	// A#B
	__m128 A_B = Mat2AdjMul(A, B);
	// X# = |D|A - B(D#C)
	__m128 X_ = _mm_sub_ps(_mm_mul_ps(detD, A), Mat2Mul(B, D_C));
	// W# = |A|D - C(A#B)
	__m128 W_ = _mm_sub_ps(_mm_mul_ps(detA, D), Mat2Mul(C, A_B));

	// |M| = |A|*|D| + ... (continue later)
	__m128 detM = _mm_mul_ps(detA, detD);

	// Y# = |B|C - D(A#B)#
	__m128 Y_ = _mm_sub_ps(_mm_mul_ps(detB, C), Mat2MulAdj(D, A_B));
	// Z# = |C|B - A(D#C)#
	__m128 Z_ = _mm_sub_ps(_mm_mul_ps(detC, B), Mat2MulAdj(A, D_C));

	// |M| = |A|*|D| + |B|*|C| ... (continue later)
	detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

	// tr((A#B)(D#C))
	__m128 tr = _mm_mul_ps(A_B, _mm_swizzle_ps_0213(D_C));
	tr = _mm_hadd_ps(tr, tr);
	tr = _mm_hadd_ps(tr, tr);
	// |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C)
	detM = _mm_sub_ps(detM, tr);

	// (1/|M|, -1/|M|, -1/|M|, 1/|M|)
	__m128 rDetM = _mm_div_ps(F32_SIGNFLIP_1110, detM);

	X_ = _mm_mul_ps(X_, rDetM);
	Y_ = _mm_mul_ps(Y_, rDetM);
	Z_ = _mm_mul_ps(Z_, rDetM);
	W_ = _mm_mul_ps(W_, rDetM);

	Mat44 r;

	// apply adjugate and store, here we combine adjugate shuffle and store shuffle
	r.cols[0] = _mm_shuffle_ps_3131(X_, Y_);
	r.cols[1] = _mm_shuffle_ps_2020(X_, Y_);
	r.cols[2] = _mm_shuffle_ps_3131(Z_, W_);
	r.cols[3] = _mm_shuffle_ps_2020(Z_, W_);

	return r;
}
#else
// for column major matrix
// we use __m128 to represent 2x2 matrix as A = | A0  A2 |
//                                              | A1  A3 |
// 2x2 column major Matrix multiply A*B
__forceinline __m128 Mat2Mul(__m128 vec1, __m128 vec2)
{
	return
		_mm_add_ps(_mm_mul_ps(vec1, _mm_swizzle_ps_0033(vec2)),
			_mm_mul_ps(_mm_swizzle_ps_2301(vec1), _mm_swizzle_ps_1122(vec2)));
}
// 2x2 column major Matrix adjugate multiply (A#)*B
__forceinline __m128 Mat2AdjMul(__m128 vec1, __m128 vec2)
{
	return
		_mm_sub_ps(_mm_mul_ps(_mm_swizzle_ps_3030(vec1), vec2),
			_mm_mul_ps(_mm_swizzle_ps_2121(vec1), _mm_swizzle_ps_1032(vec2)));

}
// 2x2 column major Matrix multiply adjugate A*(B#)
__forceinline __m128 Mat2MulAdj(__m128 vec1, __m128 vec2)
{
	return
		_mm_sub_ps(_mm_mul_ps(vec1, _mm_swizzle_ps_3300(vec2)),
			_mm_mul_ps(_mm_swizzle_ps_2301(vec1), _mm_swizzle_ps_1122(vec2)));
}

// Inverse function is the same no matter column major or row major
// this version treats it as column major
inline Mat44 GetInverse(const Mat44& inM)
{
	// use block matrix method
	// A is a matrix, then i(A) or iA means inverse of A, A# (or A_ in code) means adjugate of A, |A| (or detA in code) is determinant, tr(A) is trace

	// sub matrices
	__m128 A = _mm_shuffle_ps_0101(inM.cols[0], inM.cols[1]);
	__m128 C = _mm_shuffle_ps_2323(inM.cols[0], inM.cols[1]);
	__m128 B = _mm_shuffle_ps_0101(inM.cols[2], inM.cols[3]);
	__m128 D = _mm_shuffle_ps_2323(inM.cols[2], inM.cols[3]);

#if 0
	__m128 detA = _mm_set1_ps(inM.m[0][0] * inM.m[1][1] - inM.m[0][1] * inM.m[1][0]);
	__m128 detC = _mm_set1_ps(inM.m[0][2] * inM.m[1][3] - inM.m[0][3] * inM.m[1][2]);
	__m128 detB = _mm_set1_ps(inM.m[2][0] * inM.m[3][1] - inM.m[2][1] * inM.m[3][0]);
	__m128 detD = _mm_set1_ps(inM.m[2][2] * inM.m[3][3] - inM.m[2][3] * inM.m[3][2]);
#else
	// determinant as (|A| |C| |B| |D|)
	__m128 detSub = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps_0202(inM.cols[0], inM.cols[2]), _mm_shuffle_ps_1313(inM.cols[1], inM.cols[3])),
		_mm_mul_ps(_mm_shuffle_ps_1313(inM.cols[0], inM.cols[2]), _mm_shuffle_ps_0202(inM.cols[1], inM.cols[3]))
	);
	__m128 detA = _mm_swizzle_ps_0(detSub);
	__m128 detC = _mm_swizzle_ps_1(detSub);
	__m128 detB = _mm_swizzle_ps_2(detSub);
	__m128 detD = _mm_swizzle_ps_3(detSub);
#endif

	// let iM = 1/|M| * | X  Y |
	//                  | Z  W |

	// D#C
	__m128 D_C = Mat2AdjMul(D, C);
	// A#B
	__m128 A_B = Mat2AdjMul(A, B);
	// X# = |D|A - B(D#C)
	__m128 X_ = _mm_sub_ps(_mm_mul_ps(detD, A), Mat2Mul(B, D_C));
	// W# = |A|D - C(A#B)
	__m128 W_ = _mm_sub_ps(_mm_mul_ps(detA, D), Mat2Mul(C, A_B));

	// |M| = |A|*|D| + ... (continue later)
	__m128 detM = _mm_mul_ps(detA, detD);

	// Y# = |B|C - D(A#B)#
	__m128 Y_ = _mm_sub_ps(_mm_mul_ps(detB, C), Mat2MulAdj(D, A_B));
	// Z# = |C|B - A(D#C)#
	__m128 Z_ = _mm_sub_ps(_mm_mul_ps(detC, B), Mat2MulAdj(A, D_C));

	// |M| = |A|*|D| + |B|*|C| ... (continue later)
	detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

	// tr((A#B)(D#C))
	__m128 tr = _mm_mul_ps(A_B, _mm_swizzle_ps_0213(D_C));
	tr = _mm_hadd_ps(tr, tr);
	tr = _mm_hadd_ps(tr, tr);
	// |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C))
	detM = _mm_sub_ps(detM, tr);

	const __m128 adjSignMask = _mm_setr_ps(1.f, -1.f, -1.f, 1.f);
	// (1/|M|, -1/|M|, -1/|M|, 1/|M|)
	__m128 rDetM = _mm_div_ps(adjSignMask, detM);

	X_ = _mm_mul_ps(X_, rDetM);
	Y_ = _mm_mul_ps(Y_, rDetM);
	Z_ = _mm_mul_ps(Z_, rDetM);
	W_ = _mm_mul_ps(W_, rDetM);

	Mat44 r;

	// apply adjugate and store, here we combine adjugate shuffle and store shuffle
	r.col0 = _mm_shuffle_ps_3131(X_, Z_);
	r.col1 = _mm_shuffle_ps_2020(X_, Z_);
	r.col2 = _mm_shuffle_ps_3131(Y_, W_);
	r.col3 = _mm_shuffle_ps_2020(Y_, W_);

	return r;
}
#endif
#endif
#pragma endregion

#pragma region(eric_matrix_inversion_og)
#if 0
#define MakeShuffleMask(x,y,z,w)           (x | (y<<2) | (z<<4) | (w<<6))

// vec(0, 1, 2, 3) -> (vec[x], vec[y], vec[z], vec[w])
#define VecSwizzleMask(vec, mask)          _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), mask))
#define VecSwizzle(vec, x, y, z, w)        VecSwizzleMask(vec, MakeShuffleMask(x,y,z,w))
#define VecSwizzle1(vec, x)                VecSwizzleMask(vec, MakeShuffleMask(x,x,x,x))
// special swizzle
#define VecSwizzle_0022(vec)               _mm_moveldup_ps(vec)
#define VecSwizzle_1133(vec)               _mm_movehdup_ps(vec)

// return (vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffle(vec1, vec2, x,y,z,w)    _mm_shuffle_ps(vec1, vec2, MakeShuffleMask(x,y,z,w))
// special shuffle
#define VecShuffle_0101(vec1, vec2)        _mm_movelh_ps(vec1, vec2)
#define VecShuffle_2323(vec1, vec2)        _mm_movehl_ps(vec2, vec1)

typedef Mat44 Matrix4;
#define mVec cols


// Requires this matrix to be transform matrix, NoScale version requires this matrix be of scale 1
inline Matrix4 GetTransformInverseNoScale(const Matrix4& inM)
{
	Matrix4 r;

	// transpose 3x3, we know m03 = m13 = m23 = 0
	__m128 t0 = VecShuffle_0101(inM.mVec[0], inM.mVec[1]); // 00, 01, 10, 11
	__m128 t1 = VecShuffle_2323(inM.mVec[0], inM.mVec[1]); // 02, 03, 12, 13
	r.mVec[0] = VecShuffle(t0, inM.mVec[2], 0, 2, 0, 3); // 00, 10, 20, 23(=0)
	r.mVec[1] = VecShuffle(t0, inM.mVec[2], 1, 3, 1, 3); // 01, 11, 21, 23(=0)
	r.mVec[2] = VecShuffle(t1, inM.mVec[2], 0, 2, 2, 3); // 02, 12, 22, 23(=0)

	// last line
	r.mVec[3] = _mm_mul_ps(r.mVec[0], _mm_swizzle_ps_0(inM.mVec[3]));
	r.mVec[3] = _mm_add_ps(r.mVec[3], _mm_mul_ps(r.mVec[1], _mm_swizzle_ps_1(inM.mVec[3])));
	r.mVec[3] = _mm_add_ps(r.mVec[3], _mm_mul_ps(r.mVec[2], _mm_swizzle_ps_2(inM.mVec[3])));
	r.mVec[3] = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), r.mVec[3]);

	return r;
}


#define SMALL_NUMBER		(1.e-8f)

// Requires this matrix to be transform matrix
inline Matrix4 GetTransformInverse(const Matrix4& inM)
{
	Matrix4 r;

	// transpose 3x3, we know m03 = m13 = m23 = 0
	__m128 t0 = VecShuffle_0101(inM.mVec[0], inM.mVec[1]); // 00, 01, 10, 11
	__m128 t1 = VecShuffle_2323(inM.mVec[0], inM.mVec[1]); // 02, 03, 12, 13
	r.mVec[0] = VecShuffle(t0, inM.mVec[2], 0, 2, 0, 3); // 00, 10, 20, 23(=0)
	r.mVec[1] = VecShuffle(t0, inM.mVec[2], 1, 3, 1, 3); // 01, 11, 21, 23(=0)
	r.mVec[2] = VecShuffle(t1, inM.mVec[2], 0, 2, 2, 3); // 02, 12, 22, 23(=0)

	// (SizeSqr(mVec[0]), SizeSqr(mVec[1]), SizeSqr(mVec[2]), 0)
	__m128 sizeSqr;
	sizeSqr = _mm_mul_ps(r.mVec[0], r.mVec[0]);
	sizeSqr = _mm_add_ps(sizeSqr, _mm_mul_ps(r.mVec[1], r.mVec[1]));
	sizeSqr = _mm_add_ps(sizeSqr, _mm_mul_ps(r.mVec[2], r.mVec[2]));

	// optional test to avoid divide by 0
	__m128 one = _mm_set1_ps(1.f);
	// for each component, if(sizeSqr < SMALL_NUMBER) sizeSqr = 1;
	__m128 rSizeSqr = _mm_blendv_ps(
		_mm_div_ps(one, sizeSqr),
		one,
		_mm_cmplt_ps(sizeSqr, _mm_set1_ps(SMALL_NUMBER))
	);

	r.mVec[0] = _mm_mul_ps(r.mVec[0], rSizeSqr);
	r.mVec[1] = _mm_mul_ps(r.mVec[1], rSizeSqr);
	r.mVec[2] = _mm_mul_ps(r.mVec[2], rSizeSqr);

	// last line
	r.mVec[3] = _mm_mul_ps(r.mVec[0], VecSwizzle1(inM.mVec[3], 0));
	r.mVec[3] = _mm_add_ps(r.mVec[3], _mm_mul_ps(r.mVec[1], VecSwizzle1(inM.mVec[3], 1)));
	r.mVec[3] = _mm_add_ps(r.mVec[3], _mm_mul_ps(r.mVec[2], VecSwizzle1(inM.mVec[3], 2)));
	r.mVec[3] = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), r.mVec[3]);

	return r;
}

// for row major matrix
// we use __m128 to represent 2x2 matrix as A = | A0  A1 |
//                                              | A2  A3 |
// 2x2 row major Matrix multiply A*B
__forceinline __m128 Mat2Mul(__m128 vec1, __m128 vec2)
{
	return
		_mm_add_ps(_mm_mul_ps(vec1, VecSwizzle(vec2, 0, 3, 0, 3)),
			_mm_mul_ps(VecSwizzle(vec1, 1, 0, 3, 2), VecSwizzle(vec2, 2, 1, 2, 1)));
}
// 2x2 row major Matrix adjugate multiply (A#)*B
__forceinline __m128 Mat2AdjMul(__m128 vec1, __m128 vec2)
{
	return
		_mm_sub_ps(_mm_mul_ps(VecSwizzle(vec1, 3, 3, 0, 0), vec2),
			_mm_mul_ps(VecSwizzle(vec1, 1, 1, 2, 2), VecSwizzle(vec2, 2, 3, 0, 1)));

}
// 2x2 row major Matrix multiply adjugate A*(B#)
__forceinline __m128 Mat2MulAdj(__m128 vec1, __m128 vec2)
{
	return
		_mm_sub_ps(_mm_mul_ps(vec1, VecSwizzle(vec2, 3, 0, 3, 0)),
			_mm_mul_ps(VecSwizzle(vec1, 1, 0, 3, 2), VecSwizzle(vec2, 2, 1, 2, 1)));
}
// Inverse function is the same no matter column major or row major
// this version treats it as row major
inline Matrix4 GetInverse(const Matrix4& inM)
{
	// use block matrix method
	// A is a matrix, then i(A) or iA means inverse of A, A# (or A_ in code) means adjugate of A, |A| (or detA in code) is determinant, tr(A) is trace

	// sub matrices
	__m128 A = VecShuffle_0101(inM.mVec[0], inM.mVec[1]);
	__m128 B = VecShuffle_2323(inM.mVec[0], inM.mVec[1]);
	__m128 C = VecShuffle_0101(inM.mVec[2], inM.mVec[3]);
	__m128 D = VecShuffle_2323(inM.mVec[2], inM.mVec[3]);

#if 0
	__m128 detA = _mm_set1_ps(inM.m[0][0] * inM.m[1][1] - inM.m[0][1] * inM.m[1][0]);
	__m128 detB = _mm_set1_ps(inM.m[0][2] * inM.m[1][3] - inM.m[0][3] * inM.m[1][2]);
	__m128 detC = _mm_set1_ps(inM.m[2][0] * inM.m[3][1] - inM.m[2][1] * inM.m[3][0]);
	__m128 detD = _mm_set1_ps(inM.m[2][2] * inM.m[3][3] - inM.m[2][3] * inM.m[3][2]);
#else
	// determinant as (|A| |B| |C| |D|)
	__m128 detSub = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps_0202(inM.mVec[0], inM.mVec[2]), _mm_shuffle_ps_1313(inM.mVec[1], inM.mVec[3])),
		_mm_mul_ps(_mm_shuffle_ps_1313(inM.mVec[0], inM.mVec[2]), _mm_shuffle_ps_0202(inM.mVec[1], inM.mVec[3]))
	);
	__m128 detA = _mm_swizzle_ps_0(detSub);
	__m128 detB = _mm_swizzle_ps_1(detSub);
	__m128 detC = _mm_swizzle_ps_2(detSub);
	__m128 detD = _mm_swizzle_ps_3(detSub);
#endif

	// let iM = 1/|M| * | X  Y |
	//                  | Z  W |

	// D#C
	__m128 D_C = Mat2AdjMul(D, C);
	// A#B
	__m128 A_B = Mat2AdjMul(A, B);
	// X# = |D|A - B(D#C)
	__m128 X_ = _mm_sub_ps(_mm_mul_ps(detD, A), Mat2Mul(B, D_C));
	// W# = |A|D - C(A#B)
	__m128 W_ = _mm_sub_ps(_mm_mul_ps(detA, D), Mat2Mul(C, A_B));

	// |M| = |A|*|D| + ... (continue later)
	__m128 detM = _mm_mul_ps(detA, detD);

	// Y# = |B|C - D(A#B)#
	__m128 Y_ = _mm_sub_ps(_mm_mul_ps(detB, C), Mat2MulAdj(D, A_B));
	// Z# = |C|B - A(D#C)#
	__m128 Z_ = _mm_sub_ps(_mm_mul_ps(detC, B), Mat2MulAdj(A, D_C));

	// |M| = |A|*|D| + |B|*|C| ... (continue later)
	detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

	// tr((A#B)(D#C))
	__m128 tr = _mm_mul_ps(A_B, _mm_swizzle_ps_0213(D_C));
	tr = _mm_hadd_ps(tr, tr);
	tr = _mm_hadd_ps(tr, tr);
	// |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C)
	detM = _mm_sub_ps(detM, tr);

	const __m128 adjSignMask = _mm_setr_ps(1.f, -1.f, -1.f, 1.f);
	// (1/|M|, -1/|M|, -1/|M|, 1/|M|)
	__m128 rDetM = _mm_div_ps(adjSignMask, detM);

	X_ = _mm_mul_ps(X_, rDetM);
	Y_ = _mm_mul_ps(Y_, rDetM);
	Z_ = _mm_mul_ps(Z_, rDetM);
	W_ = _mm_mul_ps(W_, rDetM);

	Matrix4 r;

	// apply adjugate and store, here we combine adjugate shuffle and store shuffle
	r.mVec[0] = _mm_shuffle_ps_3131(X_, Y_);
	r.mVec[1] = _mm_shuffle_ps_2020(X_, Y_);
	r.mVec[2] = _mm_shuffle_ps_3131(Z_, W_);
	r.mVec[3] = _mm_shuffle_ps_2020(Z_, W_);

	return r;
}
#undef mVec
#endif
#pragma endregion

// Copied and stripped from GetInverse
inline __m128 Det44(const Mat44& inM)
{
#if 0 // 1 for row major matrix, 0 for column major matrix
	// sub matrices
	__m128 A = _mm_shuffle_ps_0101(inM.cols[0], inM.cols[1]);
	__m128 B = _mm_shuffle_ps_2323(inM.cols[0], inM.cols[1]);
	__m128 C = _mm_shuffle_ps_0101(inM.cols[2], inM.cols[3]);
	__m128 D = _mm_shuffle_ps_2323(inM.cols[2], inM.cols[3]);

#if 0
	__m128 detA = _mm_set1_ps(inM.m[0][0] * inM.m[1][1] - inM.m[0][1] * inM.m[1][0]);
	__m128 detB = _mm_set1_ps(inM.m[0][2] * inM.m[1][3] - inM.m[0][3] * inM.m[1][2]);
	__m128 detC = _mm_set1_ps(inM.m[2][0] * inM.m[3][1] - inM.m[2][1] * inM.m[3][0]);
	__m128 detD = _mm_set1_ps(inM.m[2][2] * inM.m[3][3] - inM.m[2][3] * inM.m[3][2]);
#else
	// determinant as (|A| |B| |C| |D|)
	__m128 detSub = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps_0202(inM.cols[0], inM.cols[2]), _mm_shuffle_ps_1313(inM.cols[1], inM.cols[3])),
		_mm_mul_ps(_mm_shuffle_ps_1313(inM.cols[0], inM.cols[2]), _mm_shuffle_ps_0202(inM.cols[1], inM.cols[3]))
	);
	__m128 detA = _mm_swizzle_ps_0(detSub);
	__m128 detB = _mm_swizzle_ps_1(detSub);
	__m128 detC = _mm_swizzle_ps_2(detSub);
	__m128 detD = _mm_swizzle_ps_3(detSub);
#endif

	// let iM = 1/|M| * | X  Y |
	//                  | Z  W |

	// D#C
	__m128 D_C = Mat2AdjMul(D, C);
	// A#B
	__m128 A_B = Mat2AdjMul(A, B);

	// |M| = |A|*|D| + ... (continue later)
	__m128 detM = _mm_mul_ps(detA, detD);

	// |M| = |A|*|D| + |B|*|C| ... (continue later)
	detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

	// tr((A#B)(D#C))
	__m128 tr = _mm_mul_ps(A_B, _mm_swizzle_ps_0213(D_C));
	tr = _mm_hadd_ps(tr, tr);
	tr = _mm_hadd_ps(tr, tr);
	// |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C)
	detM = _mm_sub_ps(detM, tr);

	return detM;
#else
	// use block matrix method
	// A is a matrix, then i(A) or iA means inverse of A, A# (or A_ in code) means adjugate of A, |A| (or detA in code) is determinant, tr(A) is trace

	// sub matrices
	__m128 A = _mm_shuffle_ps_0101(inM.cols[0], inM.cols[1]);
	__m128 C = _mm_shuffle_ps_2323(inM.cols[0], inM.cols[1]);
	__m128 B = _mm_shuffle_ps_0101(inM.cols[2], inM.cols[3]);
	__m128 D = _mm_shuffle_ps_2323(inM.cols[2], inM.cols[3]);

#if 0
	__m128 detA = _mm_set1_ps(inM.m[0][0] * inM.m[1][1] - inM.m[0][1] * inM.m[1][0]);
	__m128 detC = _mm_set1_ps(inM.m[0][2] * inM.m[1][3] - inM.m[0][3] * inM.m[1][2]);
	__m128 detB = _mm_set1_ps(inM.m[2][0] * inM.m[3][1] - inM.m[2][1] * inM.m[3][0]);
	__m128 detD = _mm_set1_ps(inM.m[2][2] * inM.m[3][3] - inM.m[2][3] * inM.m[3][2]);
#else
	// determinant as (|A| |C| |B| |D|)
	__m128 detSub = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps_0202(inM.cols[0], inM.cols[2]), _mm_shuffle_ps_1313(inM.cols[1], inM.cols[3])),
		_mm_mul_ps(_mm_shuffle_ps_1313(inM.cols[0], inM.cols[2]), _mm_shuffle_ps_0202(inM.cols[1], inM.cols[3]))
	);
	__m128 detA = _mm_swizzle_ps_0(detSub);
	__m128 detC = _mm_swizzle_ps_1(detSub);
	__m128 detB = _mm_swizzle_ps_2(detSub);
	__m128 detD = _mm_swizzle_ps_3(detSub);
#endif

	// let iM = 1/|M| * | X  Y |
	//                  | Z  W |

	// D#C
	__m128 D_C = Mat2AdjMul(D, C);
	// A#B
	__m128 A_B = Mat2AdjMul(A, B);

	// |M| = |A|*|D| + ... (continue later)
	__m128 detM = _mm_mul_ps(detA, detD);

	// |M| = |A|*|D| + |B|*|C| ... (continue later)
	detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

	// tr((A#B)(D#C))
	__m128 tr = _mm_mul_ps(A_B, _mm_swizzle_ps_0213(D_C));
	tr = _mm_hadd_ps(tr, tr);
	tr = _mm_hadd_ps(tr, tr);
	// |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C))
	detM = _mm_sub_ps(detM, tr);

	return detM;
#endif
}

const Mat44 MAT44_IDENTITY = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

extern "C"
{

	DLL Mat44 Mat44Identity()
	{
		return MAT44_IDENTITY;
	}

	DLL Mat44 Mat44Translate(const float x, const float y, const float z)
	{
		Mat44 m = MAT44_IDENTITY;
		m.col3 = _mm_set_ps(1.0f, z, y, x);
		return m;
	}

	DLL Mat44 Mat44RotateX(const float radians)
	{
		float sa = sinf(radians);
		float ca = cosf(radians);
		return { F32_UNIT_X, _mm_set_ps(0.0f, sa, ca, 0.0f), _mm_set_ps(0.0f, ca, -sa, 0.0f) , F32_UNIT_W };
	}
	DLL Mat44 Mat44RotateY(const float radians)
	{
		float sa = sinf(radians);
		float ca = cosf(radians);
		return { _mm_set_ps(0.0f, -sa, 0.0f, ca), F32_UNIT_Y, _mm_set_ps(0.0f, ca, 0.0f, sa), F32_UNIT_W };
	}
	DLL Mat44 Mat44RotateZ(const float radians)
	{
		float sa = sinf(radians);
		float ca = cosf(radians);
		return { _mm_set_ps(0.0f, 0.0f, sa, ca) , _mm_set_ps(0.0f, 0.0f, ca, -sa), F32_UNIT_Z, F32_UNIT_W };
	}
	DLL Mat44 Mat44Scale(const float x, const float y, const float z)
	{
		Mat44 m = MAT44_IDENTITY;
		m.m00 = x;
		m.m11 = y;
		m.m22 = z;
		return m;
	}
	DLL Mat44 Mat44Scale2(const __m128 scale)
	{
		Mat44 m = MAT44_IDENTITY;
		m.m00 = scale.m128_f32[0];
		m.m11 = scale.m128_f32[1];
		m.m22 = scale.m128_f32[2];
		return m;
	}
	DLL Mat44 Mat44Mul(const Mat44 rhs, const Mat44 lhs)
	{
		Mat44 m;

		m.col0 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(lhs.col0, _mm_swizzle_ps_0(rhs.col0)),
			_mm_mul_ps(lhs.col1, _mm_swizzle_ps_1(rhs.col0))),
			_mm_add_ps(_mm_mul_ps(lhs.col2, _mm_swizzle_ps_2(rhs.col0)),
				_mm_mul_ps(lhs.col3, _mm_swizzle_ps_3(rhs.col0))));

		m.col1 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(lhs.col0, _mm_swizzle_ps_0(rhs.col1)),
			_mm_mul_ps(lhs.col1, _mm_swizzle_ps_1(rhs.col1))),
			_mm_add_ps(_mm_mul_ps(lhs.col2, _mm_swizzle_ps_2(rhs.col1)),
				_mm_mul_ps(lhs.col3, _mm_swizzle_ps_3(rhs.col1))));

		m.col2 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(lhs.col0, _mm_swizzle_ps_0(rhs.col2)),
			_mm_mul_ps(lhs.col1, _mm_swizzle_ps_1(rhs.col2))),
			_mm_add_ps(_mm_mul_ps(lhs.col2, _mm_swizzle_ps_2(rhs.col2)),
				_mm_mul_ps(lhs.col3, _mm_swizzle_ps_3(rhs.col2))));

		m.col3 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(lhs.col0, _mm_swizzle_ps_0(rhs.col3)),
			_mm_mul_ps(lhs.col1, _mm_swizzle_ps_1(rhs.col3))),
			_mm_add_ps(_mm_mul_ps(lhs.col2, _mm_swizzle_ps_2(rhs.col3)),
				_mm_mul_ps(lhs.col3, _mm_swizzle_ps_3(rhs.col3))));

		return m;
	}
	DLL Mat44 Mat44Rotate(const float radiansX, const float radiansY, const float radiansZ, const ERotateOrder rotateOrder)
	{
		// TODO: we can probably optimize this, even if it's just a giant switch statement with all the operations inlined & hoping that that results in better compiler optimization
		Mat44 rotations[] = { Mat44RotateX(radiansX), Mat44RotateY(radiansY), Mat44RotateZ(radiansZ) };
		int ro = (int)rotateOrder;
		return Mat44Mul(Mat44Mul(rotations[(ro >> 4)], rotations[(ro >> 2) & 0b11]), rotations[ro & 0b11]);
	}
	DLL Mat44 Mat44Rotate2(const __m128 radians, const ERotateOrder rotateOrder)
	{
		// overloads for Mat44Rotate
		return Mat44Rotate(radians.m128_f32[0], radians.m128_f32[1], radians.m128_f32[2], rotateOrder);
	}
	DLL Mat44 EulerToMat44(const __m128 radians, const ERotateOrder rotateOrder)
	{
		// alias for Mat44Rotate2
		return Mat44Rotate(radians.m128_f32[0], radians.m128_f32[1], radians.m128_f32[2], rotateOrder);
	}

	// general mat44 inverse, use the faster versions if you can, it can save over 60%!
	DLL Mat44 Mat44Inversed(const Mat44 m)
	{
		return GetInverse(m);
	}

	// assumes matrix is (orthagonal?) transformation matrix, TODO: check shearing
	DLL Mat44 Mat44InversedFast(const Mat44 m)
	{
		return GetTransformInverse(m);
	}

	// assumes matrix is orthonormalized
	DLL Mat44 Mat44InversedFastNoScale(const Mat44 m)
	{
		return GetTransformInverseNoScale(m);
	}
	DLL Mat44 Mat44Transposed(const Mat44 _m)
	{
		Mat44 m = _m;
		_MM_TRANSPOSE4_PS(m.col0, m.col1, m.col2, m.col3);
		return m;
	}
	DLL float Mat44Determinant(const Mat44 m)
	{
		return Det44(m).m128_f32[0];
	}
	DLL Vec Mat44VectorTransform(const Mat44 m, const __m128 v)
	{
		// Note: set v.w to 0 to ignore translation
		return { _mm_add_ps(_mm_add_ps(_mm_mul_ps(m.col0, _mm_swizzle_ps_0(v)),
			_mm_mul_ps(m.col1, _mm_swizzle_ps_1(v))),
			_mm_add_ps(_mm_mul_ps(m.col2, _mm_swizzle_ps_2(v)),
				_mm_mul_ps(m.col3, _mm_swizzle_ps_3(v)))) };
	}

	// TODO: rotate order support
	DLL Vec Mat44ToEuler(const Mat44 m, const ERotateOrder ro)
	{
		/*
		TODO:
		There is something off with these results but I'm not sure
		if the cause is Mat44ToQuat or QuatToEuler
		*/
#if 1
		// https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToEuler/index.htm
		// This is decomposed with ERotateOrder::XZY
		float heading, attitude, bank;
		if (ro == ERotateOrder::XZY)
		{
			if (m.m01 > 0.998f) // singularity at north pole
			{
				heading = atan2f(m.m02, m.m22);
				attitude = HALF_PI;
				bank = 0.0f;
				return { _mm_set_ps(0.0f, attitude, heading, bank) };
			}
			if (m.m01 < -0.998f) // singularity at south pole
			{
				heading = atan2f(m.m02, m.m22);
				attitude = -HALF_PI;
				bank = 0.0f;
				return { _mm_set_ps(0.0f, attitude, heading, bank) };
			}
			heading = atan2f(-m.m02, m.m00);
			attitude = asinf(m.m01);
			bank = atan2f(-m.m21, m.m11);
			return { _mm_set_ps(0.0f, attitude, heading, bank) };
		}
#endif
		Quat q = Mat44ToQuat(m);
		return QuatToEuler(q, ro);
#if 0
		// We only need 5 matrix values to decompose the matrix
		// which depend on the rotate order, so let's get some indices  to help
		int headingIndex = (int)ro & 3;
		int pitchIndex = ((int)ro >> 2) & 3;
		int rollIndex = (int)ro >> 4;

		// The roll is a bit complicated as it gets the first row
		// except for the rollIndex column.
		// I'm not really sure why this works.
		int a = (pitchIndex << 2) + headingIndex;
		int b = (headingIndex << 2) + headingIndex;
		// The rollIndex is applied last so that column points into the right direction to extract yaw and pitch from
		// pitch
		int c = (rollIndex << 2) + headingIndex;
		// heading
		int d = (rollIndex << 2) + pitchIndex;
		int e = (rollIndex << 2) + rollIndex;
		// singularity
		int f = (pitchIndex << 2) + pitchIndex;

		// Now there are some necessary sign flips to make the angles work, I am not even sure why they're so inconsistent.
		int sa = 1, sb = 1, sc = 1, sd = 1, se = 1, sf = 1;
		switch (ro)
		{
		case ERotateOrder::XYZ:
		case ERotateOrder::ZXY:
			sc = -1; break;
		case ERotateOrder::YZX: // Here C is off by 100 degrees, I had this issue with the other cases but it could be fixed by flipping the sign of one of 2 atan arguments, but with asin we don't have that. I tried using every possible atan in the 3x3 matrix but could not find a correct result.
			sa = -1; sb = -1; sc = -1; sd = -1; se = -1; break;
		case ERotateOrder::XZY:
			sb = -1; se = -1; break;
		case ERotateOrder::YXZ:
			sa = -1; sd = -1; break;
			sa = -1; sd = -1; break;
		}

		// Now let's decompose!
		float r[3];
		if (sc * m.m[c] > 0.998f) // singularity at north pole
		{
			r[0] = atan2f(-sa * m.m[a], sf * m.m[f]);
			r[1] = HALF_PI;
			r[2] = 0.0f;
		}
		else if (sc * m.m[c] < -0.998f) // singularity at south pole
		{
			r[0] = atan2f(-sa * m.m[a], sf * m.m[f]);
			r[1] = -HALF_PI;
			r[2] = 0.0f;
		}
		else
		{
			r[0] = atan2f(sa * m.m[a], sb * m.m[b]);
			r[1] = asinf(sc * m.m[c]);
			r[2] = atan2f(sd * m.m[d], se * m.m[e]);
		}

		// Then we must re-order, which again has two exceptions
		// Cycle left
		if (ro == ERotateOrder::YZX)
		{
			float x = r[1], y = r[2], z = r[0];
			r[0] = x; r[1] = y; r[2] = z;
		}

		// Cycle right
		if (ro == ERotateOrder::ZXY)
		{
			float x = r[2], y = r[0], z = r[1];
			r[0] = x; r[1] = y; r[2] = z;
		}

		return _mm_set_ps(0.0f, r[headingIndex], r[pitchIndex], r[rollIndex]);
#endif
	}

	static inline Mat44 _Mat44AxisAngle(const __m128 _axis, const float cosAngle, const float sinAngle)
	{
		// Since all kronos' math notations have somehow died this is useless:
		// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml
		// Luckily there is a german delphi wiki that wraps GL AND properly copied it's docs
		// https://wiki.delphigl.com/index.php/glRotate
		__m128 axis = Vec3Normalized(_axis, F32_UNIT_X); // this ensures w = 0

		__m128 scaledAxis, sinAxis;
		Mat44 r;

		scaledAxis = _mm_set_ps1(1.0f - cosAngle);
		scaledAxis = _mm_mul_ps(axis, scaledAxis);

		sinAxis = _mm_set_ps1(sinAngle);
		sinAxis = _mm_mul_ps(axis, _mm_set_ps1(sinAngle));
		sinAxis.m128_f32[3] = 1.0f; // todo: this is probably bad

		r.col0 = _mm_set_ps1(axis.m128_f32[0]);
		r.col0 = _mm_mul_ps(r.col0, scaledAxis);
		r.col1 = _mm_swizzle_ps_3210(sinAxis); // just use col1 while we dont need it
		r.col1 = _mm_mul_ps(F32_SIGNFLIP_VEC3_010, r.col1);
		r.col0 = _mm_add_ps(r.col0, r.col1);

		r.col1 = _mm_set_ps1(axis.m128_f32[1]);
		r.col1 = _mm_mul_ps(r.col1, scaledAxis);
		r.col2 = _mm_swizzle_ps_2301(sinAxis); // just use col2 while we dont need it
		r.col2 = _mm_mul_ps(F32_SIGNFLIP_VEC3_001, r.col2);
		r.col1 = _mm_add_ps(r.col1, r.col2);

		r.col2 = _mm_set_ps1(axis.m128_f32[2]);
		r.col2 = _mm_mul_ps(r.col2, scaledAxis);
		r.col3 = _mm_swizzle_ps_1032(sinAxis); // juse use col3 while we dont need it
		r.col3 = _mm_mul_ps(F32_SIGNFLIP_VEC3_100, r.col3);
		r.col2 = _mm_add_ps(r.col2, r.col3);

		r.col3 = F32_UNIT_W;

		return r;
	}
	DLL Mat44 Mat44AxisAngle(const __m128 axis, const float radians)
	{
		// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml
		return _Mat44AxisAngle(axis, cosf(radians), sinf(radians));
	}
	DLL Mat44 Mat44Align(const __m128 from, const __m128 to)
	{
		__m128 axis = Vec3Cross(from, to);
		float cosAngle = Vec3Dot(from, to);
		float sinAngle = sqrtf(1.0f - cosAngle * cosAngle);
		return _Mat44AxisAngle(axis, cosAngle, sinAngle);
	}
	DLL Mat44 Mat44RotateTowards(const __m128 from, const __m128 to)
	{
		// alias for Align
		return Mat44Align(from, to);
	}
	DLL Mat44 Mat44LookAt(const __m128 targetDirection, const __m128 _upDirection, const EAxis forwardAxis, const EAxis upAxis)
	{
		__m128 right = Vec3Cross(targetDirection, _upDirection);
		__m128 upDirection = Vec3Cross(targetDirection, right);
		Mat44 r;
		int z = (int)forwardAxis & 0b11;
		r.cols[z] = ((int)forwardAxis - z != 0) ? _mm_neg_ps(targetDirection) : targetDirection;
		int y = (int)upAxis & 0b11;
		r.cols[y] = ((int)upAxis - y != 0) ? _mm_neg_ps(upDirection) : upDirection;
		int x = y ^ z;
		r.cols[x] = Vec3Cross(upDirection, r.cols[z]);
		r.col3 = F32_UNIT_W;
		return r;
	}
	DLL Mat44 Mat44FromVectors(const __m128 c0, const __m128 c1, const __m128 c2, const __m128 translate)
	{
		return { c0, c1, c2, translate };
	}
	DLL Mat44 Mat44ToTop33(const Mat44 _m)
	{
		Mat44 m = _m;
		// simply set the translation column to UNIT_W
		m.col3 = F32_UNIT_W;
		return m;
	}
	// TODO: Order of operations are unclear while writing this doc, make sure delta * newParent = m
	DLL Mat44 Mat44Delta(const Mat44 m, const Mat44 newParent)
	{
		// Get this matrix in the space of the other, so that multiply newParent yields the input m
		return Mat44Mul(m, Mat44Inversed(newParent));
	}
	DLL Mat44 Mat44TranslateRotate(const float x, const float y, const float z, const float radiansX, const float radiansY, const float radiansZ, const ERotateOrder rotateOrder)
	{
		Mat44 r = Mat44Rotate(radiansX, radiansY, radiansZ, rotateOrder);
		r.col3 = _mm_set_ps(1.0f, z, y, x);
		return r;
	}
	DLL Mat44 Mat44TranslateRotate2(const __m128 translate, const __m128 radians, const ERotateOrder rotateOrder)
	{
		Mat44 r = Mat44Rotate2(radians, rotateOrder);
		r.col3 = _mm_add_ps(_mm_mul_ps(translate, F32_VEC3_MASK), F32_UNIT_W); // translate.w = translate.w * 0 + 1
		return r;
	}
	DLL Mat44 Mat44TRS(const float x, const float y, const float z, const float radiansX, const float radiansY, const float radiansZ, const float scaleX, const float scaleY, const float scaleZ, const ERotateOrder rotateOrder)
	{
		Mat44 r = Mat44TranslateRotate(x, y, z, radiansX, radiansY, radiansZ, rotateOrder);
		r.col0 = _mm_mul_ps(r.col0, _mm_set_ps1(scaleX));
		r.col1 = _mm_mul_ps(r.col1, _mm_set_ps1(scaleY));
		r.col2 = _mm_mul_ps(r.col2, _mm_set_ps1(scaleZ));
		return r;
	}
	DLL Mat44 Mat44TRS2(const __m128 translate, const __m128 radians, const __m128 scale, const ERotateOrder rotateOrder)
	{
		Mat44 r = Mat44TranslateRotate2(translate, radians, rotateOrder);
		r.col0 = _mm_mul_ps(r.col0, _mm_set_ps1(scale.m128_f32[0]));
		r.col1 = _mm_mul_ps(r.col1, _mm_set_ps1(scale.m128_f32[1]));
		r.col2 = _mm_mul_ps(r.col2, _mm_set_ps1(scale.m128_f32[2]));
		return r;
	}
	DLL Mat44 Mat44Parented(const Mat44 child, const Mat44 parent)
	{
		// The whole point is that it is an alias of Mat44Mul with the SAME ARGUMENT ORDER and the argument names help remind us how it works
		return Mat44Mul(child, parent);
	}

	// Matrix validation
	// We return 0 bit if validation passed on check not requested, so basically  if the result is 0 you're good and else you can use the enum to find out what validation failed.
	static inline bool fequals(float a, float b, float e) { return fabs(a - b) < e; }
	DLL Mat44ValidationFlags Mat44Validate(const Mat44 m, const Mat44ValidationFlags flags, const float epsilon)
	{
		// check if axes are perpendicular
		int f = (int)flags;

		if ((f & (int)Mat44ValidationFlags::Orthagonal) != 0)
		{
			if (fequals(Vec3Dot(m.col0, m.col1), 0.0f, epsilon) &&
				fequals(Vec3Dot(m.col1, m.col2), 0.0f, epsilon) &&
				fequals(Vec3Dot(m.col2, m.col0), 0.0f, epsilon))
			{
				// passed!
				f ^= (int)Mat44ValidationFlags::Orthagonal;
			}
		}

		if ((f & (int)Mat44ValidationFlags::Normalized) != 0)
		{
			if (fequals(Vec3SqrMagnitude(m.col0), 1.0f, epsilon) &&
				fequals(Vec3SqrMagnitude(m.col1), 1.0f, epsilon) &&
				fequals(Vec3SqrMagnitude(m.col2), 1.0f, epsilon))
			{
				// passed!
				f ^= (int)Mat44ValidationFlags::Normalized;
			}
		}

		if ((f & (int)Mat44ValidationFlags::Uniform) != 0)
		{
			float magnitude = Vec3SqrMagnitude(m.col0);
			if (fequals(Vec3SqrMagnitude(m.col1), magnitude, epsilon) &&
				fequals(Vec3SqrMagnitude(m.col2), magnitude, epsilon))
			{
				// passed!
				f ^= (int)Mat44ValidationFlags::Uniform;
			}
		}

		if ((f & (int)Mat44ValidationFlags::NotFlipped) != 0)
		{
			if (Vec3Dot(Vec3Cross(m.col0, m.col1), m.col2) > 0.0f)
			{
				// passed!
				f ^= (int)Mat44ValidationFlags::NotFlipped;
			}
		}

		if ((f & (int)Mat44ValidationFlags::FourthRow) != 0)
		{
			__m128 row3 = _mm_shuffle_ps_1313(_mm_shuffle_ps_2323(m.col0, m.col1), _mm_shuffle_ps_2323(m.col2, m.col3));
			__m128 equality = _mm_abs_ps(_mm_sub_ps(row3, F32_UNIT_W));
			if (equality.m128_f32[0] < epsilon && equality.m128_f32[1] < epsilon && equality.m128_f32[2] < epsilon && equality.m128_f32[3] < epsilon)
			{
				// passed!
				f ^= (int)Mat44ValidationFlags::FourthRow;
			}
		}

		return (Mat44ValidationFlags)f;
	}
	DLL Mat44 Mat44MakeValid(const Mat44 _m, const Mat44ValidationFlags flags)
	{
		Mat44 m = _m;
		int f = (int)flags;

		if ((f & (int)Mat44ValidationFlags::Orthagonal) != 0)
		{
			// simplly re-derive the axes from col0
			m.col2 = Vec3Cross(m.col0, m.col1);
			m.col1 = Vec3Cross(m.col0, m.col2);
		}

		if ((f & (int)Mat44ValidationFlags::Uniform) != 0)
		{
			__m128 averageMagnitude = _mm_set_ps1((Vec3Magnitude(m.col0) + Vec3Magnitude(m.col1) + Vec3Magnitude(m.col2)) / 3.0f);
			m.col0 = _mm_mul_ps(Vec3Normalized(m.col0, F32_UNIT_X), averageMagnitude);
			m.col1 = _mm_mul_ps(Vec3Normalized(m.col1, F32_UNIT_Y), averageMagnitude);
			m.col2 = _mm_mul_ps(Vec3Normalized(m.col2, F32_UNIT_Z), averageMagnitude);
		}
		else if ((f & (int)Mat44ValidationFlags::Normalized) != 0)
		{
			m.col0 = Vec3Normalized(m.col0, F32_UNIT_X);
			m.col1 = Vec3Normalized(m.col1, F32_UNIT_Y);
			m.col2 = Vec3Normalized(m.col2, F32_UNIT_Z);
		}

		if ((f & (int)Mat44ValidationFlags::NotFlipped) != 0)
		{
			if (Vec3Dot(Vec3Cross(m.col0, m.col1), m.col2) < 0.0f)
			{
				m.col2 = _mm_neg_ps(m.col2);
			}
		}

		if ((f & (int)Mat44ValidationFlags::FourthRow) != 0)
		{
			m.m03 = 0.0f;
			m.m13 = 0.0f;
			m.m23 = 0.0f;
			m.m33 = 1.0f;
		}

		return m;
	}
	DLL Vec Mat44ToScale(const Mat44 m)
	{
		// decompose scale
		return { _mm_set_ps(0.0f,
			Vec3Magnitude(m.col2),
			Vec3Magnitude(m.col1),
			Vec3Magnitude(m.col0)) };
	}
	DLL Vec Mat44ToTranslate(const Mat44 m)
	{
		// decompose translate
		return { _mm_mul_ps(F32_VEC3_MASK, m.col3) };
	}
	DLL Mat44 Mat44Frustum(const float left, const float right, const float bottom, const float top, const float near, const float far)
	{
		float dx = right - left;
		float dy = top - bottom;
		float dz = far - near;
		return { (2.0f * near) / dx, 0.0f, 0.0f, 0.0f,
			0.0f, (2.0f * near) / dy, 0.0f, 0.0f,
			(right + left) / dx, (top + bottom) / dy, (far + near) / dz,  -1.0f,
			0.0f, 0.0f, (2.0f * far * near) / dz, 0.0f };
	}
	DLL Mat44 Mat44PerspectiveX(const float horizontalFieldOfViewRadians, const float aspectRatio, const float near, const float far)
	{
		float halfWidth = tanf(0.5f * horizontalFieldOfViewRadians) * near;
		float halfHeight = halfWidth / aspectRatio;
		return Mat44Frustum(-halfWidth, halfWidth, -halfHeight, halfHeight, near, far);
	}
	DLL Mat44 Mat44PerspectiveY(const float verticalFieldOfViewRadians, const float aspectRatio, const float near, const float far)
	{
		float halfHeight = tanf(0.5f * verticalFieldOfViewRadians) * near;
		float halfWidth = halfHeight * aspectRatio;
		return Mat44Frustum(-halfWidth, halfWidth, -halfHeight, halfHeight, near, far);
	}
	DLL Mat44 Mat44Orthographic(const float left, const float right, const float top, const float bottom, const float near, const float far)
	{
		float dx = right - left;
		float dy = top - bottom;
		float dz = far - near;
		float x = (right + left) / dx;
		float y = (top + bottom) / dy;
		float z = (far + near) / dz;
		return { 2.0f / dx, 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / dy, 0.0f, 0.0f,
			0.0f, 0.0f, -2.0f / dz, 0.0f,
			x, y, z, 1.0f };
	}
	DLL Mat44 Mat44OrthoSymmetric(const float width, const float height, const float near, const float far)
	{
		float dz = far - near;
		return { width, 0.0f, 0.0f, 0.0f,
			0.0f, height, 0.0f, 0.0f,
			0.0f, 0.0f, -2.0f / dz, 0.0f,
			0.0f, 0.0f, (far + near) / dz, 1.0f };
	}
}
