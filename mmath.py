"""
MMath - vector math library for 3D applications.
Released under the MIT License:

Copyright 2020 Trevor van Hoof

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
"""
import os
import ctypes
from MMath import menum
from math import pi as PI

_instance = None


class ERotateOrder(menum.Enum, int):
    XYZ = 0b000110
    YZX = 0b011000
    ZXY = 0b100001
    XZY = 0b001001
    YXZ = 0b010010
    ZYX = 0b100100

    @classmethod
    def from_param(cls, *args):
        return ctypes.c_int.from_param(*args)


class EAxis(menum.Enum, int):
    X = 0b000
    Y = 0b001
    Z = 0b010
    NEG_X = 0b100
    NEG_Y = 0b101
    NEG_Z = 0b110

    @classmethod
    def from_param(cls, *args):
        return ctypes.c_int.from_param(*args)


class Mat44ValidationFlags(menum.Enum, int):
    ValidationOK = 0b00000
    Orthagonal = 0b00001
    Normalized = 0b00010
    Uniform = 0b00100
    NotFlipped = 0b01000
    FourthRow = 0b10000

    @classmethod
    def from_param(cls, *args):
        return ctypes.c_int.from_param(*args)


def _dll():
    global _instance
    if _instance is not None:
        return _instance
    _instance = ctypes.WinDLL(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'x64', 'DLLRelease', 'MMath.dll'))
    # Mat44.h
    _instance.Mat44Identity.argtypes = tuple()
    _instance.Mat44Identity.restype = Mat44
    _instance.Mat44Translate.argtypes = (ctypes.c_float, ctypes.c_float, ctypes.c_float)
    _instance.Mat44Translate.restype = Mat44
    _instance.Mat44RotateX.argtypes = (ctypes.c_float,)
    _instance.Mat44RotateX.restype = Mat44
    _instance.Mat44RotateY.argtypes = (ctypes.c_float,)
    _instance.Mat44RotateY.restype = Mat44
    _instance.Mat44RotateZ.argtypes = (ctypes.c_float,)
    _instance.Mat44RotateZ.restype = Mat44
    _instance.Mat44Scale.argtypes = (ctypes.c_float, ctypes.c_float, ctypes.c_float)
    _instance.Mat44Scale.restype = Mat44
    _instance.Mat44Scale2.argtypes = (Float4,)
    _instance.Mat44Scale2.restype = Mat44
    _instance.Mat44Mul.argtypes = (Mat44, Mat44)
    _instance.Mat44Mul.restype = Mat44
    _instance.Mat44Inversed.argtypes = (Mat44,)
    _instance.Mat44Inversed.restype = Mat44
    _instance.Mat44InversedFast.argtypes = (Mat44,)
    _instance.Mat44InversedFast.restype = Mat44
    _instance.Mat44InversedFastNoScale.argtypes = (Mat44,)
    _instance.Mat44InversedFastNoScale.restype = Mat44
    _instance.Mat44Transposed.argtypes = (Mat44,)
    _instance.Mat44Transposed.restype = Mat44
    _instance.Mat44Determinant.argtypes = (Mat44,)
    _instance.Mat44Determinant.restype = ctypes.c_float
    _instance.Mat44VectorTransform.argtypes = (Mat44, Float4)
    _instance.Mat44VectorTransform.restype = Float4
    _instance.Mat44ToEuler.argtypes = (Mat44, ERotateOrder)
    _instance.Mat44ToEuler.restype = Float4
    _instance.Mat44AxisAngle.argtypes = (Float4, ctypes.c_float)
    _instance.Mat44AxisAngle.restype = Mat44
    _instance.Mat44Align.argtypes = (Float4, Float4)
    _instance.Mat44Align.restype = Mat44
    _instance.Mat44RotateTowards.argtypes = (Float4, Float4)
    _instance.Mat44RotateTowards.restype = Mat44
    _instance.Mat44LookAt.argtypes = (Float4, Float4, EAxis, EAxis)
    _instance.Mat44LookAt.restype = Mat44
    _instance.Mat44FromVectors.argtypes = (Float4, Float4, Float4, Float4)
    _instance.Mat44FromVectors.restype = Mat44
    _instance.Mat44ToTop33.argtypes = (Mat44,)
    _instance.Mat44ToTop33.restype = Mat44
    _instance.Mat44Delta.argtypes = (Mat44, Mat44)
    _instance.Mat44Delta.restype = Mat44
    _instance.Mat44Rotate.argtypes = (ctypes.c_float, ctypes.c_float, ctypes.c_float, ERotateOrder)
    _instance.Mat44Rotate.restype = Mat44
    _instance.Mat44Rotate2.argtypes = (Float4, ERotateOrder)
    _instance.Mat44Rotate2.restype = Mat44
    _instance.EulerToMat44.argtypes = (Float4, ERotateOrder)
    _instance.EulerToMat44.restype = Mat44
    _instance.Mat44TranslateRotate.argtypes = (ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ERotateOrder)
    _instance.Mat44TranslateRotate.restype = Mat44
    _instance.Mat44TranslateRotate2.argtypes = (Float4, Float4, ERotateOrder)
    _instance.Mat44TranslateRotate2.restype = Mat44
    _instance.Mat44TRS.argtypes = (ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ERotateOrder)
    _instance.Mat44TRS.restype = Mat44
    _instance.Mat44TRS2.argtypes = (Float4, Float4, Float4, ERotateOrder)
    _instance.Mat44TRS2.restype = Mat44
    _instance.Mat44Parented.argtypes = (Mat44, Mat44)
    _instance.Mat44Parented.restype = Mat44
    _instance.Mat44Validate.argtypes = (Mat44, Mat44ValidationFlags, ctypes.c_float)
    _instance.Mat44Validate.restype = Mat44ValidationFlags
    _instance.Mat44MakeValid.argtypes = (Mat44, Mat44ValidationFlags)
    _instance.Mat44MakeValid.restype = Mat44
    _instance.Mat44ToScale.argtypes = (Mat44,)
    _instance.Mat44ToScale.restype = Float4
    _instance.Mat44ToTranslate.argtypes = (Mat44,)
    _instance.Mat44ToTranslate.restype = Float4
    _instance.Mat44Frustum.argtypes = (ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float)
    _instance.Mat44Frustum.restype = Mat44
    _instance.Mat44PerspectiveX.argtypes = (ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float)
    _instance.Mat44PerspectiveX.restype = Mat44
    _instance.Mat44PerspectiveY.argtypes = (ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float)
    _instance.Mat44PerspectiveY.restype = Mat44
    _instance.Mat44Orthographic.argtypes = (ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float)
    _instance.Mat44Orthographic.restype = Mat44
    _instance.Mat44OrthoSymmetric.argtypes = (ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float)
    _instance.Mat44OrthoSymmetric.restype = Mat44
    # Quat.h
    _instance.QuatIdentity.argtypes = tuple()
    _instance.QuatIdentity.restype = Quat
    _instance.QuatRotateX.argtypes = (ctypes.c_float,)
    _instance.QuatRotateX.restype = Quat
    _instance.QuatRotateY.argtypes = (ctypes.c_float,)
    _instance.QuatRotateY.restype = Quat
    _instance.QuatRotateZ.argtypes = (ctypes.c_float,)
    _instance.QuatRotateZ.restype = Quat
    _instance.QuatMul.argtypes = (Quat, Quat)
    _instance.QuatMul.restype = Quat
    _instance.QuatDot.argtypes = (Quat, Quat)
    _instance.QuatDot.restype = ctypes.c_float
    _instance.QuatSqrMagnitude.argtypes = (Quat,)
    _instance.QuatSqrMagnitude.restype = ctypes.c_float
    _instance.QuatMagnitude.argtypes = (Quat,)
    _instance.QuatMagnitude.restype = ctypes.c_float
    _instance.QuatNormalized.argtypes = (Quat, Quat)
    _instance.QuatNormalized.restype = Quat
    _instance.QuatInversed.argtypes = (Quat,)
    _instance.QuatInversed.restype = Quat
    _instance.QuatConjugated.argtypes = (Quat,)
    _instance.QuatConjugated.restype = Quat
    _instance.QuatSlerp.argtypes = (Quat, Quat, ctypes.c_float)
    _instance.QuatSlerp.restype = Quat
    _instance.QuatVectorTransform.argtypes = (Quat, Float4)
    _instance.QuatVectorTransform.restype = Float4
    _instance.QuatToEuler.argtypes = (Quat, ERotateOrder)
    _instance.QuatToEuler.restype = Float4
    # Vector.h
    _instance.Vec4Dot.argtypes = (Float4, Float4)
    _instance.Vec4Dot.restype = ctypes.c_float
    _instance.Vec4SqrMagnitude.argtypes = (Float4,)
    _instance.Vec4SqrMagnitude.restype = ctypes.c_float
    _instance.Vec4Magnitude.argtypes = (Float4,)
    _instance.Vec4Magnitude.restype = ctypes.c_float
    _instance.Vec4Normalized.argtypes = (Float4, Float4)
    _instance.Vec4Normalized.restype = Vec4
    _instance.Vec4NormalizedUnsafe.argtypes = (Float4,)
    _instance.Vec4NormalizedUnsafe.restype = Vec4
    _instance.Vec4Perpendicular.argtypes = (Float4,)
    _instance.Vec4Perpendicular.restype = Vec4
    _instance.Vec3Dot.argtypes = (Float4, Float4)
    _instance.Vec3Dot.restype = ctypes.c_float
    _instance.Vec3Cross.argtypes = (Float4, Float4)
    _instance.Vec3Cross.restype = Vec3
    _instance.Vec3SqrMagnitude.argtypes = (Float4,)
    _instance.Vec3SqrMagnitude.restype = ctypes.c_float
    _instance.Vec3Magnitude.argtypes = (Float4,)
    _instance.Vec3Magnitude.restype = ctypes.c_float
    _instance.Vec3Normalized.argtypes = (Float4, Float4)
    _instance.Vec3Normalized.restype = Vec3
    _instance.Vec3NormalizedUnsafe.argtypes = (Float4,)
    _instance.Vec3NormalizedUnsafe.restype = Vec3
    _instance.Vec3Perpendicular.argtypes = (Float4,)
    _instance.Vec3Perpendicular.restype = Vec3
    _instance.Vec2Dot.argtypes = (Float4, Float4)
    _instance.Vec2Dot.restype = ctypes.c_float
    _instance.Vec2Cross.argtypes = (Float4, Float4)
    _instance.Vec2Cross.restype = ctypes.c_float
    _instance.Vec2SqrMagnitude.argtypes = (Float4,)
    _instance.Vec2SqrMagnitude.restype = ctypes.c_float
    _instance.Vec2Magnitude.argtypes = (Float4,)
    _instance.Vec2Magnitude.restype = ctypes.c_float
    _instance.Vec2Normalized.argtypes = (Float4, Float4)
    _instance.Vec2Normalized.restype = Vec2
    _instance.Vec2NormalizedUnsafe.argtypes = (Float4,)
    _instance.Vec2NormalizedUnsafe.restype = Vec2
    _instance.Vec2Perpendicular.argtypes = (Float4,)
    _instance.Vec2Perpendicular.restype = Vec2
    # Friends.h
    _instance.QuatToMat44.argtypes = (Quat,)
    _instance.QuatToMat44.restype = Mat44
    _instance.Mat44ToQuat.argtypes = (Mat44,)
    _instance.Mat44ToQuat.restype = Quat
    # Vector.cpp
    _instance.VecAdd.argtypes = (Float4, Float4)
    _instance.VecAdd.restype = Float4
    _instance.VecSub.argtypes = (Float4, Float4)
    _instance.VecSub.restype = Float4
    _instance.VecMul.argtypes = (Float4, Float4)
    _instance.VecMul.restype = Float4
    _instance.VecDiv.argtypes = (Float4, Float4)
    _instance.VecDiv.restype = Float4
    _instance.VecMin.argtypes = (Float4, Float4)
    _instance.VecMin.restype = Float4
    _instance.VecMax.argtypes = (Float4, Float4)
    _instance.VecMax.restype = Float4
    _instance.VecAbs.argtypes = (Float4,)
    _instance.VecAbs.restype = Float4
    _instance.VecSign.argtypes = (Float4,)
    _instance.VecSign.restype = Float4
    _instance.VecNegate.argtypes = (Float4,)
    _instance.VecNegate.restype = Float4
    _instance.VecSin.argtypes = (Float4,)
    _instance.VecSin.restype = Float4
    _instance.VecCos.argtypes = (Float4,)
    _instance.VecCos.restype = Float4
    _instance.VecFloor.argtypes = (Float4,)
    _instance.VecFloor.restype = Float4
    _instance.VecCeil.argtypes = (Float4,)
    _instance.VecCeil.restype = Float4
    _instance.VecRound.argtypes = (Float4,)
    _instance.VecRound.restype = Float4

    return _instance


_float4 = ctypes.c_float * 4


class Float4(ctypes.Structure):
    _fields_ = [('m', _float4)]

    def __init__(self, *args):
        if args:
            if len(args) == 1:
                if isinstance(args[0], Float4):
                    super(Float4, self).__init__(args[0].m)
                else:
                    s = args[0]
                    super(Float4, self).__init__(_float4(s, s, s, s))
            else:
                m = _float4()
                for i, v in enumerate(args):
                    m[i] = ctypes.c_float(v)
                super(Float4, self).__init__(m)
        else:
            super(Float4, self).__init__(*args)

    def __hash__(self):
        return hash((self.x, self.y, self.z, self.w))

    def __str__(self):
        return '%.05f, %.05f, %.05f, %.05f' % tuple(self.m)

    def toTupleXYZ(self):
        return float(self.m[0]), float(self.m[1]), float(self.m[2])

    def toTuple(self):
        return float(self.m[0]), float(self.m[1]), float(self.m[2]), float(self.m[3])

    def __getitem__(self, item):
        return self.m[item]

    def __setitem__(self, item, value):
        self.m[item] = value

    @property
    def x(self):
        return self.m[0]

    @property
    def y(self):
        return self.m[1]

    @property
    def z(self):
        return self.m[2]

    @property
    def w(self):
        return self.m[3]

    @x.setter
    def x(self, v):
        self.m[0] = v

    @y.setter
    def y(self, v):
        self.m[1] = v

    @z.setter
    def z(self, v):
        self.m[2] = v

    @w.setter
    def w(self, v):
        self.m[3] = v

    def __add__(self, rhs):
        if isinstance(rhs, float): rhs = Float4(rhs)
        return self.__class__(_dll().VecAdd(self, rhs))

    def __sub__(self, rhs):
        if isinstance(rhs, float): rhs = Float4(rhs)
        return self.__class__(_dll().VecSub(self, rhs))

    def __mul__(self, rhs):
        if isinstance(rhs, float): rhs = Float4(rhs)
        return self.__class__(_dll().VecMul(self, rhs))

    def __div__(self, rhs):
        if isinstance(rhs, float): rhs = Float4(rhs)
        return self.__class__(_dll().VecDiv(self, rhs))

    def __neg__(self):
        return self.__class__(_dll().VecNegate(self))

    def min(self, rhs):
        if isinstance(rhs, float): rhs = Float4(rhs)
        return _dll().VecMin(self, rhs)

    def max(self, rhs):
        if isinstance(rhs, float): rhs = Float4(rhs)
        return _dll().VecMax(self, rhs)

    def abs(self):
        return _dll().VecAbs(self)

    def sign(self):
        return _dll().VecSign(self)

    def sin(self):
        return _dll().VecSin(self)

    def cos(self):
        return _dll().VecCos(self)

    def floor(self):
        return _dll().VecFloor(self)

    def ceil(self):
        return _dll().VecCeil(self)

    def round(self):
        return _dll().VecRound(self)


class Mat44(ctypes.Union):
    _fields_ = (('m', ctypes.c_float * 16),
                ('cols', Float4 * 4))

    def __str__(self):
        return '%.05f, %.05f, %.05f, %.05f\n%.05f, %.05f, %.05f, %.05f\n%.05f, %.05f, %.05f, %.05f\n%.05f, %.05f, %.05f, %.05f' % tuple(
            self.m)

    # Access behaviour to mirror the C++ union
    @property
    def col0(self):
        return self.cols[0]

    @property
    def col1(self):
        return self.cols[1]

    @property
    def col2(self):
        return self.cols[2]

    @property
    def col3(self):
        return self.cols[3]

    @property
    def m00(self):
        return self.m[0]

    @property
    def m01(self):
        return self.m[1]

    @property
    def m02(self):
        return self.m[2]

    @property
    def m03(self):
        return self.m[3]

    @property
    def m10(self):
        return self.m[4]

    @property
    def m11(self):
        return self.m[5]

    @property
    def m12(self):
        return self.m[6]

    @property
    def m13(self):
        return self.m[7]

    @property
    def m20(self):
        return self.m[8]

    @property
    def m21(self):
        return self.m[9]

    @property
    def m22(self):
        return self.m[10]

    @property
    def m23(self):
        return self.m[11]

    @property
    def m30(self):
        return self.m[12]

    @property
    def m31(self):
        return self.m[13]

    @property
    def m32(self):
        return self.m[14]

    @property
    def m33(self):
        return self.m[15]

    @col0.setter
    def col0(self, v):
        self.cols[0] = v

    @col1.setter
    def col1(self, v):
        self.cols[1] = v

    @col2.setter
    def col2(self, v):
        self.cols[2] = v

    @col3.setter
    def col3(self, v):
        self.cols[3] = v

    @m00.setter
    def m00(self, v):
        self.m[0] = v

    @m01.setter
    def m01(self, v):
        self.m[1] = v

    @m02.setter
    def m02(self, v):
        self.m[2] = v

    @m03.setter
    def m03(self, v):
        self.m[3] = v

    @m10.setter
    def m10(self, v):
        self.m[4] = v

    @m11.setter
    def m11(self, v):
        self.m[5] = v

    @m12.setter
    def m12(self, v):
        self.m[6] = v

    @m13.setter
    def m13(self, v):
        self.m[7] = v

    @m20.setter
    def m20(self, v):
        self.m[8] = v

    @m21.setter
    def m21(self, v):
        self.m[9] = v

    @m22.setter
    def m22(self, v):
        self.m[10] = v

    @m23.setter
    def m23(self, v):
        self.m[11] = v

    @m30.setter
    def m30(self, v):
        self.m[12] = v

    @m31.setter
    def m31(self, v):
        self.m[13] = v

    @m32.setter
    def m32(self, v):
        self.m[14] = v

    @m33.setter
    def m33(self, v):
        self.m[15] = v

    # Wrap the DLL in a more pythonic way
    @staticmethod
    def identity():
        return _dll().Mat44Identity()

    @staticmethod
    def translate(x, y, z):
        return _dll().Mat44Translate(x, y, z)

    @staticmethod
    def rotateX(radians):
        return _dll().Mat44RotateX(radians)

    @staticmethod
    def rotateY(radians):
        return _dll().Mat44RotateY(radians)

    @staticmethod
    def rotateZ(radians):
        return _dll().Mat44RotateZ(radians)

    @staticmethod
    def scale(x, y, z):
        return _dll().Mat44Scale(x, y, z)

    @staticmethod
    def scale2(scale):
        return _dll().Mat44Scale2(scale)

    def mul(self, other):
        return _dll().Mat44Mul(self, other)

    def __mul__(self, other):
        return _dll().Mat44Mul(self, other)

    def __imul__(self, other):
        self.m = (self * other).m
        return self

    def inversed(self):
        return _dll().Mat44Inversed(self)

    def inversedFast(self):
        return _dll().Mat44InversedFast(self)

    def inversedFastNoScale(self):
        return _dll().Mat44InversedFastNoScale(self)

    def transposed(self):
        return _dll().Mat44Transposed(self)

    def determinant(self):
        return _dll().Mat44Determinant(self)

    def vectorTransform(self, v):
        return _dll().Mat44VectorTransform(self, v)

    def toEuler(self, ro):
        return _dll().Mat44ToEuler(self, ro)

    @staticmethod
    def axisAngle(axis, radians):
        return _dll().Mat44AxisAngle(axis, radians)

    @staticmethod
    def align(_from, to):
        return _dll().Mat44Align(_from, to)

    @staticmethod
    def rotateTowards(_from, to):
        return _dll().Mat44RotateTowards(_from, to)

    @staticmethod
    def lookAt(targetDirection, upDirection, forward, upAxis):
        return _dll().Mat44LookAt(targetDirection, upDirection, forward, upAxis)

    @staticmethod
    def fromVectors(c0, c1, c2, translate):
        return _dll().Mat44FromVectors(c0, c1, c2, translate)

    def toTop33(self):
        return _dll().Mat44ToTop33(self)

    def delta(self, newParent):
        return _dll().Mat44Delta(self, newParent)

    @staticmethod
    def rotate(radiansX, radiansY, radiansZ, rotateOrder):
        return _dll().Mat44Rotate(radiansX, radiansY, radiansZ, rotateOrder)

    @staticmethod
    def rotate2(radians, rotateOrder):
        return _dll().Mat44Rotate2(radians, rotateOrder)

    @staticmethod
    def toMat44(radians, rotateOrder):
        return _dll().EulerToMat44(radians, rotateOrder)

    @staticmethod
    def translateRotate(x, y, z, radiansX, radiansY, radiansZ, rotateOrder):
        return _dll().Mat44TranslateRotate(x, y, z, radiansX, radiansY, radiansZ, rotateOrder)

    @staticmethod
    def translateRotate2(translate, radians, rotateOrder):
        return _dll().Mat44TranslateRotate2(translate, radians, rotateOrder)

    @staticmethod
    def TRS(x, y, z, radiansX, radiansY, radiansZ, scaleX, scaleY, scaleZ, rotateOrder):
        return _dll().Mat44TRS(x, y, z, radiansX, radiansY, radiansZ, scaleX, scaleY, scaleZ, rotateOrder)

    @staticmethod
    def TRS2(translate, radians, scale, rotateOrder):
        return _dll().Mat44TRS2(translate, radians, scale, rotateOrder)

    def parented(self, parent):
        return _dll().Mat44Parented(self, parent)

    def validate(self, flags, epsilon):
        return _dll().Mat44Validate(self, flags, epsilon)

    def makeValid(self, flags):
        return _dll().Mat44MakeValid(self, flags)

    def toScale(self):
        return _dll().Mat44ToScale(self)

    def toTranslate(self):
        return _dll().Mat44ToTranslate(self)

    @staticmethod
    def frustum(left, right, top, bottom, near, far):
        return _dll().Mat44Frustum(left, right, top, bottom, near, far)

    @staticmethod
    def perspectiveX(horizontalFieldOfViewRadians, aspectRatio, near, far):
        return _dll().Mat44PerspectiveX(horizontalFieldOfViewRadians, aspectRatio, near, far)

    @staticmethod
    def perspectiveY(verticalFieldOfViewRadians, aspectRatio, near, far):
        return _dll().Mat44PerspectiveY(verticalFieldOfViewRadians, aspectRatio, near, far)

    @staticmethod
    def orthographic(left, right, top, bottom, near, far):
        return _dll().Mat44Orthographic(left, right, top, bottom, near, far)

    @staticmethod
    def orthoSymmetric(width, height, near, far):
        return _dll().Mat44OrthoSymmetric(width, height, near, far)

    def toQuat(self):
        return _dll().Mat44ToQuat(self)


class Quat(Float4):
    @staticmethod
    def identity():
        return _dll().QuatIdentity()

    @staticmethod
    def rotateX(radians):
        return _dll().QuatRotateX(radians)

    @staticmethod
    def rotateY(radians):
        return _dll().QuatRotateY(radians)

    @staticmethod
    def rotateZ(radians):
        return _dll().QuatRotateZ(radians)

    def mul(self, rhs):
        return _dll().QuatMul(self, rhs)

    def dot(self, b):
        return _dll().QuatDot(self, b)

    def sqrMagnitude(self):
        return _dll().QuatSqrMagnitude(self)

    def magnitude(self):
        return _dll().QuatMagnitude(self)

    def normalized(self, fallback):
        return _dll().QuatNormalized(self, fallback)

    def inversed(self):
        return _dll().QuatInversed(self)

    def conjugated(self):
        return _dll().QuatConjugated(self)

    def slerp(self, r, t):
        return _dll().QuatSlerp(self, r, t)

    def vectorTransform(self, v):
        return _dll().QuatVectorTransform(self, v)

    def toEuler(self, rotateOrder):
        return _dll().QuatToEuler(self, rotateOrder)

    def toMat44(self):
        return _dll().QuatToMat44(self)


class Vec4(Float4):
    def dot(self, b):
        return _dll().Vec4Dot(self, b)

    def sqrMagnitude(self):
        return _dll().Vec4SqrMagnitude(self)

    def magnitude(self):
        return _dll().Vec4Magnitude(self)

    def normalized(self, fallback):
        return _dll().Vec4Normalized(self, fallback)

    def normalizedUnsafe(self):
        return _dll().Vec4NormalizedUnsafe(self)

    def perpendicular(self):
        return _dll().Vec4Perpendicular(self)


class Vec3(Float4):
    def dot(self, b):
        return _dll().Vec3Dot(self, b)

    def cross(self, b):
        return _dll().Vec3Cross(self, b)

    def sqrMagnitude(self):
        return _dll().Vec3SqrMagnitude(self)

    def magnitude(self):
        return _dll().Vec3Magnitude(self)

    def normalized(self, fallback):
        return _dll().Vec3Normalized(self, fallback)

    def normalizedUnsafe(self):
        return _dll().Vec3NormalizedUnsafe(self)

    def perpendicular(self):
        return _dll().Vec3Perpendicular(self)


class Vec2(Float4):
    def dot(self, b):
        return _dll().Vec2Dot(self, b)

    def cross(self, b):
        return _dll().Vec2Cross(self, b)

    def sqrMagnitude(self):
        return _dll().Vec2SqrMagnitude(self)

    def magnitude(self):
        return _dll().Vec2Magnitude(self)

    def normalized(self, fallback):
        return _dll().Vec2Normalized(self, fallback)

    def normalizedUnsafe(self):
        return _dll().Vec2NormalizedUnsafe(self)

    def perpendicular(self):
        return _dll().Vec2Perpendicular(self)


# print Mat44.TRS(0.5, 1.5, -2.5, 0.0, 3.14159265359 * 0.5, 0.0, 1.0, 2.0, 1.0, ERotateOrder.XYZ)


TAU = PI + PI
HALF_PI = PI / 2.0
RAD2DEG = 180.0 / PI
DEG2RAD = PI / 180.0
