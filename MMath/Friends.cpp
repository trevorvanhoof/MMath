/**
MMath - vector math library for 3D applications.
Released under the MIT License:

Copyright 2020 Trevor van Hoof

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/
#include "Mat44.h"
#include "Quat.h"
#include "Math.h"
#include "Vector.h"
#include "Friends.h"
#include "Enums.h"
#include "SIMD.h"
#include <math.h>

extern "C"
{
	DLL Mat44 QuatToMat44(Quat q)
	{
		Mat44 m;
#if 1
		__m128 sqr = _mm_mul_ps(q.q, q.q);
		__m128 wq = _mm_mul_ps(_mm_swizzle_ps_3(q.q), q.q);
		__m128 alt = _mm_mul_ps(_mm_swizzle_ps_1203(q.q), q.q);

		__m128 dot = _mm_hadd_ps(sqr, sqr);
		dot = _mm_hadd_ps(dot, dot);
		__m128 s = _mm_div_ps(_mm_set_ps1(2.0f), dot);

		sqr = _mm_neg_ps(sqr);

		// s * (xy + wz)
		// s * (xy - wz)
		// s * (zx + wy)
		// s * (zx - wy)
		__m128 a = _mm_mul_ps(s, _mm_add_ps(_mm_swizzle_ps_0022(alt), _mm_mul_ps(F32_SIGNFLIP_0101, _mm_swizzle_ps_2211(wq))));
		// s * (yz + wx)
		// s * (yz - wx)
		__m128 b = _mm_mul_ps(s, _mm_add_ps(_mm_swizzle_ps_1(alt), _mm_mul_ps(F32_SIGNFLIP_0101, _mm_swizzle_ps_0(wq))));
		// 1.0 - s * (yy + zz)
		// 1.0 - s * (xx + zz)
		// 1.0 - s * (xx + yy)
		__m128 c = _mm_add_ps(F32_ONE, _mm_mul_ps(s, _mm_add_ps(_mm_swizzle_ps_1000(sqr), _mm_swizzle_ps_2211(sqr))));

		// TODO: I could not really condense this down properly, needs a do-over
		m.col0 = _mm_set_ps(0.0f, a.m128_f32[3], a.m128_f32[0], c.m128_f32[0]);
		m.col1 = _mm_set_ps(0.0f, b.m128_f32[0], c.m128_f32[1], a.m128_f32[1]);
		m.col2 = _mm_set_ps(0.0f, c.m128_f32[2], b.m128_f32[1], a.m128_f32[2]);
#endif
#if 0
		// https://github.com/Autodesk/animx/blob/master/src/internal/Tquaternion.h
		float ww = q.w * q.w, xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
		float s = 2.0f / (ww + xx + yy + zz);
		float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z, wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;

		// Use this if multiply vectors on the left (pre-multipy).
		// This just the transposed matrix of the one above.
		m.col0.m128_f32[0] = 1.0f - s * (yy + zz);
		m.col0.m128_f32[1] = s * (xy + wz);
		m.col0.m128_f32[2] = s * (xz - wy);
		m.col0.m128_f32[3] = 0.0f;

		m.col1.m128_f32[0] = s * (xy - wz);
		m.col1.m128_f32[1] = 1.0f - s * (xx + zz);
		m.col1.m128_f32[2] = s * (yz + wx);
		m.col1.m128_f32[3] = 0.0f;

		m.col2.m128_f32[0] = s * (xz + wy);
		m.col2.m128_f32[1] = s * (yz - wx);
		m.col2.m128_f32[2] = 1.0f - s * (xx + yy);
		m.col2.m128_f32[3] = 0.0f;
#endif
		m.col3 = F32_UNIT_W;
		return m;
	}

	DLL Quat Mat44ToQuat(Mat44 m)
	{
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
		float trace = m.m00 + m.m11 + m.m22;
		__m128 q;
		float S;

		if (trace > 0)
		{
			S = (trace + 1.0f);
			q = _mm_set_ps(
				S,
				m.m10 - m.m01,
				m.m02 - m.m20,
				m.m21 - m.m12);
		}
		else if ((m.m00 > m.m11) & (m.m00 > m.m22))
		{
			S = (1.0f + m.m00 - m.m11 - m.m22);
			q = _mm_set_ps(
				m.m21 - m.m12,
				m.m02 + m.m20,
				m.m01 + m.m10,
				S);
		}
		else if (m.m11 > m.m22)
		{
			S = (1.0f + m.m11 - m.m00 - m.m22);
			q = _mm_set_ps(
				m.m02 - m.m20,
				m.m12 + m.m21,
				S,
				m.m01 + m.m10);
		}
		else
		{
			S = (1.0f + m.m22 - m.m00 - m.m11);
			q = _mm_set_ps(
				m.m01 - m.m10,
				S,
				m.m12 + m.m21,
				m.m02 + m.m20);
		}

		return { _mm_mul_ps(q, _mm_rsqrt_ps(_mm_set_ps1(S + S))) };
	}
}
