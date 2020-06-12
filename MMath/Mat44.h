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
	};

	DLL Mat44 Mat44Identity();
	DLL Mat44 Mat44Translate(float x, float y, float z);
	DLL Mat44 Mat44RotateX(float radians);
	DLL Mat44 Mat44RotateY(float radians);
	DLL Mat44 Mat44RotateZ(float radians);
	DLL Mat44 Mat44Scale(float x, float y, float z);
	DLL Mat44 Mat44Scale2(__m128 scale);
	DLL Mat44 Mat44Mul(Mat44 rhs, Mat44 lhs);
	DLL Mat44 Mat44Inversed(Mat44 m);
	DLL Mat44 Mat44InversedFast(Mat44 m);
	DLL Mat44 Mat44InversedFastNoScale(Mat44 m);
	DLL Mat44 Mat44Transposed(Mat44 m);
	DLL float Mat44Determinant(Mat44 m);
	DLL Vec Mat44VectorTransform(Mat44 m, __m128 v);
	DLL Vec Mat44ToEuler(Mat44 m, ERotateOrder ro);

	DLL Mat44 Mat44AxisAngle(__m128 axis, float radians); // Rotate around a given vector
	DLL Mat44 Mat44Align(__m128 from, __m128 to); // Construct a matrix so that, when transforming 'from', the result is 'to'. Uses cross & dot to convert to axis-angle scenario.
	DLL Mat44 Mat44RotateTowards(__m128 from, __m128 to);// alias for Align
	DLL Mat44 Mat44LookAt(__m128 targetDirection, __m128 upDirection, EAxis forward, EAxis upAxis);
	DLL Mat44 Mat44FromVectors(__m128 c0, __m128 c1, __m128 c2, __m128 translate);
	DLL Mat44 Mat44ToTop33(Mat44 m); // simply set the translation column to UNIT_W
	// TODO: Order of operations are unclear while writing this doc, make sure delta * newParent = m
	DLL Mat44 Mat44Delta(Mat44 m, Mat44 newParent); // Get this matrix in the space of the other, so that multiply newParent yields the input m
	DLL Mat44 Mat44Rotate(float radiansX, float radiansY, float radiansZ, ERotateOrder rotateOrder);
	DLL Mat44 Mat44Rotate2(__m128 radians, ERotateOrder rotateOrder);
	DLL Mat44 EulerToMat44(__m128 radians, ERotateOrder rotateOrder);  // alias for Mat44Rotate2
	DLL Mat44 Mat44TranslateRotate(float x, float y, float z, float radiansX, float radiansY, float radiansZ, ERotateOrder rotateOrder);
	DLL Mat44 Mat44TranslateRotate2(__m128 translate, __m128 radians, ERotateOrder rotateOrder);
	DLL Mat44 Mat44TRS(float x, float y, float z, float radiansX, float radiansY, float radiansZ, float scaleX, float scaleY, float scaleZ, ERotateOrder rotateOrder);
	DLL Mat44 Mat44TRS2(__m128 translate, __m128 radians, __m128 scale, ERotateOrder rotateOrder);
	// The whole point is that it is an alias of Mat44Mul with the SAME ARGUMENT ORDER and the argument names help remind us how it works
	DLL Mat44 Mat44Parented(Mat44 child, Mat44 parent);
	// TODO: euler decomposition rotate order support
	DLL Mat44ValidationFlags Mat44Validate(Mat44 m, Mat44ValidationFlags flags, float epsilon); // useful for throwing warnings
	DLL Mat44 Mat44MakeValid(Mat44 m, Mat44ValidationFlags flags); // useful for rectifying warnings (at the cost of being fairly slow)
	DLL Vec Mat44ToScale(Mat44 m); // decompose scale
	DLL Vec Mat44ToTranslate(Mat44 m); // decompose translate
	DLL Mat44 Mat44Frustum(float left, float right, float top, float bottom, float near, float far);
	DLL Mat44 Mat44PerspectiveX(float horizontalFieldOfViewRadians, float aspectRatio, float near, float far);
	DLL Mat44 Mat44PerspectiveY(float verticalFieldOfViewRadians, float aspectRatio, float near, float far);
	DLL Mat44 Mat44Orthographic(float left, float right, float top, float bottom, float near, float far);
	DLL Mat44 Mat44OrthoSymmetric(float width, float height, float near, float far);
	//	DLL Mat44 Mat44Lerp(Mat44 lhs, Mat44 rhs, float t); // ? this seems hardly worthwhile and probably just becomes converting to xform, lerping that, and converting back
}
