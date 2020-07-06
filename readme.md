# TODO
Unit tests
- Ensure 100% code coverage, currently only testing matrices
- Do performance metrics by testing tons of randomized inputs

XForm struct
XFormScale struct

Quat QuatAxisAngle(__m128 axis, float radians); // Rotate around a given vector
Quat QuatAlign(__m128 from, __m128 to); // Construct a matrix so that, when transforming 'from', the result is 'to'. Uses cross & dot to convert to axis-angle scenario.
Quat QuatRotateTowards alias for Align
Quat QuatLookAt(__m128 targetDirection, __m128 upDirection, EAxis forward, EAxis upAxis)
Quat QuatDelta(Mat44 m, Mat44 newParent) // Get this matrix in the space of the other, so that multiply newParent yields the input m
Quat QuatRotate(x,y,z)
Quat QuatParent(child,parent) TODO: these inputs must reflect multiplication order, so if child * parent yields world space child we're good, but if that is the opposite we must flip the args
QuatToAxisAngle

# MMath
MMath - vector math library for 3D applications.
By Trevor van Hoof.
Released under the MIT License:

# License
Copyright 2020 Trevor van Hoof

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# About
General rules:

We use radians at all times, globals for PI, TAU, DEG2RAD and RAD2DEG are defined for you in Math.h

We aim for modern CPUs, so compile for AVX2 or expect having to implement missing intrinsics.

For matrices:
We follow OpenGL and Maya memory layout, this is to say a transformation matrix
is 4 contiguous vectors where translation occupy the 13, 14, 15 indices.

OpenGL documentation uses 'column-major' notation.
This means that when the openGL spec says a translation matrix looks like:
1 0 0 x
0 1 0 y
0 0 1 z
0 0 0 1
The in-memory representation is this:
1 0 0 0 0 1 0 0 0 0 1 0 x y z 1
(so naively inserting newlines results in something that looks transposed)

Maya documentation uses 'row-major' notation, so they put line breaks as you might
expect and note a translation matrix as:
1 0 0 0
0 1 0 0
0 0 1 0
x y z 1

Again the memory layout is 100% identical!

We post-multiply, mostly to match Maya's rotation order notation.
So rotate-order XYZ results in X * Y * Z, easier to perform.
Post multiplication means child * parent, and as Maya's translate moves the node in
parent-space it is also scale * rotate * translate. This matches the Maya documentation,
so if you were to go for the full maya implementation with pivots and shearing you
could directly refer to the multiplication order defined in MTransformationmatrix .

Our camera math also follows openGL and Maya meaning that by default the camera
looks in negative Z, Y is the default up vector and the default rotate order is XYZ
so that any Z consitutes a roll around the local forward axis (after applying X and Y).

Naturally this will all be customizable through arguments.

# Acknowledgements & Attribution
This would not exist without some great resources that I've used over the years.
Some of which I used on a regular basis while developing this to fill gaps in my knowledge or find reference implementations.

SIMD Mat44 inversion by Eric Zhang, taken from:
https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html

Quaternion to euler decomposition by Amy de Buitl�ir, referenced from:
https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/Quaternions.pdf

Legacy routines sourced from Martin John Baker, who has a very comprehensive website:
https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToEuler/index.htm
https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/arithmetic/index.htm
NOTICE! 'euclideanspace' code is released under the GPL2 and GPL3, which requires distributions in binary form to also disclose source.
All these routines have been superseded by more elaborate versions (e.g. rotate order independent and/or SIMD),
the original code remains between #if 0 defines so they will not end up in your binary, if you were to use these
legacy routines you are subject to the GPL2 or GPL3 license.

Perpendicular vector algorithm was implemented as described by Ahmed Fasih here:
https://math.stackexchange.com/questions/133177/finding-a-unit-vector-perpendicular-to-another-vector/413235#413235

Bidirectional Matrix and Quaternion conversions are referenced from

Some quaternion code was taken from Autodesk's animx library:
https://github.com/Autodesk/animx/blob/master/src/internal/Tquaternion.h
The original code is licensed under the Apache 2.0 license

This license requires me to list my modifications to Tquaternion.h, and the modified version in MMath is distributed as part of MMath and is exclusively licensed under the same license as the rest of MMath.

I have taken the following methods: 

void Tquaternion::convertToMatrix(T4dDblMatrix& tm) const
Was converted to a global c-style function taking a Quat as argument (instead of the implicit this) and made to interact with my data structures in order to compile.
Later a SIMD version was created and the resulting functions retains none of the original code but follows the same algorithm.
The original code (after making it compatible with my data structures and compile succesfully) remains in the source, preprocessed away using #if 0 so it will not end up in binaries.

Tquaternion Tquaternion::operator*(const Tquaternion& rhs) const
Was converted to a global c-style function taking a Quat as argument (instead of the implicit this) and made to interact with my data structures in order to compile.
Due to discrepancies in the noutput when I created my SIMD version I cross referenced other sources and the algorithm behind the resulting code is slightly different.
The original code (after making it compatible with my data structures and compile succesfully) remains in the source, preprocessed away using #if 0 so it will not end up in binaries.

Tquaternion slerp(const Tquaternion& p, const Tquaternion& q, double t)
Was made to interact with my data structures in order to compile.
Later a SIMD version was created and the resulting functions retains none of the original code but follows the same algorithm.
