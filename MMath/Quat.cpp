/**
MMath - vector math library for 3D applications.
Released under the MIT License:

Copyright 2020 Trevor van Hoof

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/
#include "Quat.h"
#include "SIMD.h"
#include "Math.h"
#include "Vector.h"
#include "Enums.h"
#include <math.h>

static const __m128 F32_SIGNFLIP_1110 = { -1.0f, -1.0f, -1.0f, 1.0f };
static const __m128 F32_SIGNFLIP_1001 = { -1.0f, 1.0f, 1.0f, -1.0f };

extern "C"
{
	DLL Quat QuatIdentity()
	{
		return { F32_UNIT_W };
	}
	DLL Quat QuatRotateX(float radians)
	{
		radians *= 0.5f;
		float sa = sinf(radians);
		float ca = cosf(radians);
		return { _mm_set_ps(ca, 0.0f, 0.0f, sa) };
	}
	DLL Quat QuatRotateY(float radians)
	{
		radians *= 0.5f;
		float sa = sinf(radians);
		float ca = cosf(radians);
		return { _mm_set_ps(ca, 0.0f, sa, 0.0f) };
	}
	DLL Quat QuatRotateZ(float radians)
	{
		radians *= 0.5f;
		float sa = sinf(radians);
		float ca = cosf(radians);
		return { _mm_set_ps(ca, sa, 0.0f, 0.0f) };
	}
	DLL Quat QuatMul(Quat lhs, Quat rhs)
	{
#if 0
		// https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/arithmetic/index.htm
		return { rhs.w * lhs.x + rhs.x * lhs.w + rhs.y * lhs.z - rhs.z * lhs.y,
				 rhs.w * lhs.y - rhs.x * lhs.z + rhs.y * lhs.w + rhs.z * lhs.x,
				 rhs.w * lhs.z + rhs.x * lhs.y - rhs.y * lhs.x + rhs.z * lhs.w,
				 rhs.w * lhs.w - rhs.x * lhs.x - rhs.y * lhs.y - rhs.z * lhs.z };
#endif

#if 1
		__m128 x = _mm_mul_ps(F32_SIGNFLIP_0101, _mm_mul_ps(_mm_swizzle_ps_3210(lhs.q), _mm_swizzle_ps_0(rhs.q)));
		__m128 y = _mm_mul_ps(F32_SIGNFLIP_0011, _mm_mul_ps(_mm_swizzle_ps_2301(lhs.q), _mm_swizzle_ps_1(rhs.q)));
		__m128 z = _mm_mul_ps(F32_SIGNFLIP_1001, _mm_mul_ps(_mm_swizzle_ps_1032(lhs.q), _mm_swizzle_ps_2(rhs.q)));
		__m128 w = _mm_mul_ps(lhs.q, _mm_swizzle_ps_3(rhs.q));
		return { _mm_add_ps(_mm_add_ps(x, y), _mm_add_ps(z, w)) };
#endif

#if 0
		// https://github.com/Autodesk/animx/blob/master/src/internal/Tquaternion.h
		Quat result;
		result.x = rhs.w * lhs.x + rhs.x * lhs.w + rhs.y * lhs.z - rhs.z * lhs.y;
		result.y = rhs.w * lhs.y + rhs.y * lhs.w + rhs.z * lhs.x - rhs.x * lhs.z;
		result.z = rhs.w * lhs.z + rhs.z * lhs.w + rhs.x * lhs.y - rhs.y * lhs.x;
		result.w = rhs.w * lhs.w - rhs.x * lhs.x - rhs.y * lhs.y - rhs.z * lhs.z;
		return result;
#endif
	}
	DLL float QuatDot(Quat a, Quat b)
	{
		__m128 tmp = _mm_mul_ps(a.q, b.q);
		tmp = _mm_hadd_ps(tmp, tmp);
		tmp = _mm_hadd_ps(tmp, tmp);
		return tmp.m128_f32[0];
	}
	DLL float QuatSqrMagnitude(Quat q)
	{
		return QuatDot(q, q);
	}
	DLL float QuatMagnitude(Quat q)
	{
		return sqrtf(QuatSqrMagnitude(q));
	}
	DLL Quat QuatNormalized(Quat q, Quat fallback)
	{
		__m128 tmp = _mm_mul_ps(q.q, q.q);
		tmp = _mm_hadd_ps(tmp, tmp);
		tmp = _mm_hadd_ps(tmp, tmp);
		if (tmp.m128_f32[0] == 0.0f)
			return fallback;
		return { _mm_div_ps(q.q, _mm_sqrt_ps(tmp)) };
	}
	// note, inversion only works on normalized quaternions (though post-normalize is also fine)
	DLL Quat QuatInversed(Quat q) // also known as conjugate
	{
		return { _mm_mul_ps(q.q, F32_SIGNFLIP_1110) };
	}
	DLL Quat QuatConjugated(Quat q) // also known as inverse
	{
		return QuatInversed(q);
	}
	DLL Quat QuatSlerp(Quat l, Quat r, float t)
	{
		// TODO: UNTESTED
		// https://github.com/Autodesk/animx/blob/master/src/internal/Tquaternion.h

		__m128 tmp = r.q;

		__m128 cosOmega4 = _mm_mul_ps(l.q, r.q);
		cosOmega4 = _mm_hadd_ps(cosOmega4, cosOmega4);
		cosOmega4 = _mm_hadd_ps(cosOmega4, cosOmega4);

		// Always take the shortest path
		if (cosOmega4.m128_f32[0] < 0)
		{
			cosOmega4 = _mm_neg_ps(cosOmega4);
			tmp = _mm_neg_ps(tmp);
		}

		// Standard case slerp, the tolerance is to avoid infinities
		const __m128 F32_UNSIGEND_MASK = _mm_castsi128_ps(_mm_set1_epi32(~(1 << 31)));
		cosOmega4 = _mm_and_ps(F32_UNSIGEND_MASK, cosOmega4);
		if (cosOmega4.m128_f32[0] < 0.99999f)
		{
			__m128 sinOmega4 = _mm_sqrt_ps(_mm_sub_ps(F32_ONE, _mm_mul_ps(cosOmega4, cosOmega4)));
#if (_MSC_VER < 1920)
			__m128 omega = _mm_set_ps1(acosf(cosOmega4.m128_f32[0]));
#else
			__m128 omega = _mm_acos_ps(cosOmega4);
#endif		
			__m128 angles = _mm_mul_ps(omega, _mm_set_ps(0.0f, 0.0f, t, 1.0f - t));
			__m128 weights = _mm_div_ps(_mm_sin_ps(angles), sinOmega4);
			return { _mm_add_ps(_mm_mul_ps(l.q, _mm_swizzle_ps_0(weights)), _mm_mul_ps(tmp, _mm_swizzle_ps_1(weights))) };
		}

		// Otherwise p and q are very close, fallback to linear interpolation
		return { _mm_add_ps(l.q, _mm_mul_ps(_mm_set_ps1(t), _mm_sub_ps(tmp, l.q))) };
	}
	// This w component is copied from v but otherwise ignored
	DLL Vec QuatVectorTransform(Quat q, __m128 v)
	{
		// TODO: SIMD?
		float x = v.m128_f32[0];
		float y = v.m128_f32[1];
		float z = v.m128_f32[2];
		return { _mm_set_ps(v.m128_f32[3],
			x * (-2 * q.w * q.y + 2 * q.x * q.z) + y * (2 * q.w * q.x + 2 * q.y * q.z) + z * (q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z),
			x * (2 * q.w * q.z + 2 * q.x * q.y) + y * (q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z) + z * (-2 * q.w * q.x + 2 * q.y * q.z),
			x * (q.x * q.x + q.w * q.w - q.y * q.y - q.z * q.z) + y * (2 * q.x * q.y - 2 * q.w * q.z) + z * (2 * q.x * q.z + 2 * q.w * q.y)
		) };
	}

	DLL Vec QuatToEuler(Quat q, ERotateOrder order)
	{
		// Any rotate order quaternion decomposition from:
		// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/Quaternions.pdf

		int i0 = ((int)order >> 4) & 3;
		int i1 = ((int)order >> 2) & 3;
		int i2 = (int)order & 3;

		// swizzle q to match rotate order
		float p0 = q.w, p1 = q.s[i0], p2 = q.s[i1], p3 = q.s[i2];

		static const __m128 AXES[3] = { F32_UNIT_X, F32_UNIT_NEG_Y, F32_UNIT_Z }; // TODO: I had a very narrow test case and found I had to negate Y somehow, neesd more testing.
		float e = SignNotZero(Vec3Dot(Vec3Cross(AXES[i1], AXES[i2]), AXES[i0]));
		float sinTheta1 = 2.0f * (p0 * p2 + e * p1 * p3);
		__m128 result = _mm_setzero_ps();
		result.m128_f32[i1] = asinf(sinTheta1);
		if (fabs(sinTheta1) < 0.999999f)
		{
			result.m128_f32[i0] = atan2f(2.0f * (p0 * p1 - e * p2 * p3), 1.0f - 2.0f * (p1 * p1 + p2 * p2));
			result.m128_f32[i2] = atan2f(2.0f * (p0 * p3 - e * p2 * p1), 1.0f - 2.0f * (p3 * p3 + p2 * p2));
		}
		else
		{
			result.m128_f32[i0] = atan2f(p1, p0);
		}

		return { result };

#if 0
		// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
		// This is decomposed with ERotateOrder::XZY
		float heading, attitude, bank;

		float test = q.x*q.y + q.z*q.w;
		if (test > 0.4999999f)  // singularity at north pole
		{
			heading = atan2f(q.x, q.w);
			heading += heading;
			attitude = HALF_PI;
			bank = 0.0f;
			return { _mm_set_ps(0.0f, attitude, heading, bank) };
		}
		if (test < -0.4999999f) // singularity at south pole
		{
			heading = -atan2f(q.x, q.w);
			heading += heading;
			attitude = -HALF_PI;
			bank = 0.0f;
			return { _mm_set_ps(0.0f, attitude, heading, bank) };
		}
		float sqx = q.x * q.x;
		sqx += sqx;
		float sqy = q.y * q.y;
		sqy += sqy;
		float sqz = q.z * q.z;
		sqz += sqz;
		heading = atan2f(2.0f * q.y * q.w - 2.0f * q.x * q.z, 1.0f - sqy - sqz);
		test += test;
		attitude = asinf(test);
		bank = atan2f(2.0f * (q.x * q.w - q.y * q.z), 1.0f - sqx - sqz);
		return { _mm_set_ps(0.0f, attitude, heading, bank) };
#endif
	}
}
