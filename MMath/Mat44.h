/**
MMath - vector math library for 3D applications.
Released under the MIT License:

Copyright 2020 Trevor van Hoof

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/
#pragma once

#include <xmmintrin.h>
#include "Enums.h"
#include "DLL.h"
#include "Vector.h"
#include <cstring>

extern "C"
{
	__declspec(align(16)) struct Mat44
	{
		union
		{
			__m128 cols[4];
			struct
			{
				__m128 col0;
				__m128 col1;
				__m128 col2;
				__m128 col3;
			};
			struct
			{
				float m00;
				float m01;
				float m02;
				float m03;
				float m10;
				float m11;
				float m12;
				float m13;
				float m20;
				float m21;
				float m22;
				float m23;
				float m30;
				float m31;
				float m32;
				float m33;
			};
			float m[16];
		};

		bool operator==(const Mat44& rhs) { return memcmp(m, rhs.m, sizeof(float) * 16) == 0; }
		Mat44 operator* (const Mat44& rhs);
		Vec operator* (const __m128& rhs);
	};

	DLL Mat44 Mat44Identity();
	DLL Mat44 Mat44Translate(const float x, const float y, const float z);
	DLL Mat44 Mat44RotateX(const float radians);
	DLL Mat44 Mat44RotateY(const float radians);
	DLL Mat44 Mat44RotateZ(const float radians);
	DLL Mat44 Mat44Scale(const float x, const float y, const float z);
	DLL Mat44 Mat44Scale2(const __m128 scale);
	DLL Mat44 Mat44Mul(const Mat44 rhs, const Mat44 lhs);
	DLL Mat44 Mat44Inversed(const Mat44 m);
	DLL Mat44 Mat44InversedFast(const Mat44 m);
	DLL Mat44 Mat44InversedFastNoScale(const Mat44 m);
	DLL Mat44 Mat44Transposed(const Mat44 m);
	DLL float Mat44Determinant(const Mat44 m);
	DLL Vec Mat44VectorTransform(const Mat44 m, const __m128 v);
	DLL Vec Mat44ToEuler(const Mat44 m, const ERotateOrder ro);

	DLL Mat44 Mat44AxisAngle(const __m128 axis, const float radians); // Rotate around a given vector
	DLL Mat44 Mat44Align(const __m128 from, const __m128 to); // Construct a matrix so that, when transforming 'from', the result is 'to'. Uses cross & dot to convert to axis-angle scenario.
	DLL Mat44 Mat44RotateTowards(const __m128 from, const __m128 to);// alias for Align
	DLL Mat44 Mat44LookAt(const __m128 targetDirection, const __m128 upDirection, EAxis forward, EAxis upAxis);
	DLL Mat44 Mat44FromVectors(const __m128 c0, const __m128 c1, const __m128 c2, const __m128 translate);
	DLL Mat44 Mat44ToTop33(const Mat44 m); // simply set the translation column to UNIT_W
	// TODO: Order of operations are unclear while writing this doc, make sure delta * newParent = m
	DLL Mat44 Mat44Delta(const Mat44 m, const Mat44 newParent); // Get this matrix in the space of the other, so that multiply newParent yields the input m
	DLL Mat44 Mat44Rotate(const float radiansX, const float radiansY, const float radiansZ, const ERotateOrder rotateOrder);
	DLL Mat44 Mat44Rotate2(const __m128 radians, const ERotateOrder rotateOrder);
	DLL Mat44 EulerToMat44(const __m128 radians, const ERotateOrder rotateOrder);  // alias for Mat44Rotate2
	DLL Mat44 Mat44TranslateRotate(const float x, const float y, const float z, const float radiansX, const float radiansY, const float radiansZ, const ERotateOrder rotateOrder);
	DLL Mat44 Mat44TranslateRotate2(const __m128 translate, const __m128 radians, const ERotateOrder rotateOrder);
	DLL Mat44 Mat44TRS(const float x, const float y, const float z, const float radiansX, const float radiansY, const float radiansZ, const float scaleX, const float scaleY, const float scaleZ, const ERotateOrder rotateOrder);
	DLL Mat44 Mat44TRS2(const __m128 translate, const __m128 radians, const __m128 scale, const ERotateOrder rotateOrder);
	// The whole point is that it is an alias of Mat44Mul with the SAME ARGUMENT ORDER and the argument names help remind us how it works
	DLL Mat44 Mat44Parented(const Mat44 child, const Mat44 parent);
	// TODO: euler decomposition rotate order support
	DLL Mat44ValidationFlags Mat44Validate(const Mat44 m, const Mat44ValidationFlags flags, const float epsilon); // useful for throwing warnings
	DLL Mat44 Mat44MakeValid(const Mat44 m, const Mat44ValidationFlags flags); // useful for rectifying warnings (at the cost of being fairly slow)
	DLL Vec Mat44ToScale(const Mat44 m); // decompose scale
	DLL Vec Mat44ToTranslate(const Mat44 m); // decompose translate
	DLL Mat44 Mat44Frustum(const float left, const float right, const float top, const float bottom, const float near, const float far);
	DLL Mat44 Mat44PerspectiveX(const float horizontalFieldOfViewRadians, const float aspectRatio, const float near, const float far);
	DLL Mat44 Mat44PerspectiveY(const float verticalFieldOfViewRadians, const float aspectRatio, const float near, const float far);
	DLL Mat44 Mat44Orthographic(const float left, const float right, const float top, const float bottom, const float near, const float far);
	DLL Mat44 Mat44OrthoSymmetric(const float width, const float height, const float near, const float far);
	//	DLL Mat44 Mat44Lerp(const Mat44 lhs, const Mat44 rhs, const float t); // ? this seems hardly worthwhile and probably just becomes converting to xform, lerping that, and converting back
}
