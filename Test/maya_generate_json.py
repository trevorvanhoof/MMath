class Mat44(object):
    def __init__(self, data=None):
        self.m = data or ([0.0] * 16)

    @property
    def col0(self):
        return m128(self.m[0:4])

    @property
    def col1(self):
        return m128(self.m[4:8])

    @property
    def col2(self):
        return m128(self.m[8:12])

    @property
    def col3(self):
        return m128(self.m[12:])

    @col0.setter
    def col0(self, col):
        self.m[0:4] = col.m128_f32

    @col1.setter
    def col1(self, col):
        self.m[4:8] = col.m128_f32

    @col2.setter
    def col2(self, col):
        self.m[8:12] = col.m128_f32

    @col3.setter
    def col3(self, col):
        self.m[12:] = col.m128_f32

    def __getitem__(self, index):
        return m128(self.m[index * 4: index * 4 + 4])

    def __setitem__(self, index, col):
        self.m[index * 4: index * 4 + 4] = col.m128_f32

    @property
    def cols(self):
        return self

    def __iter__(self):
        yield self.col0
        yield self.col1
        yield self.col2
        yield self.col3

    def __getattr__(self, name):
        # if name[0] == 'm' and len(name) == 3:
        return self.m[int(name[1]) * 4 + int(name[2])]
        # super(Mat44, self).__getattribute__(name)

    def __str__(self):
        return ''.join(str(col) for col in self.cols)


class m128(object):
    def __init__(self, data):
        assert len(data) == 4
        self.m128_f32 = data

    def __str__(self):
        return '%.03f, %.03f, %.03f, %.03f\n' % tuple(self.m128_f32)


from maya.OpenMaya import MMatrix, MScriptUtil, MSelectionList, MGlobal, MDagPath, MTransformationMatrix
from maya import cmds


def mat44ToMMatrix(m):
    o = MMatrix()
    MScriptUtil.createMatrixFromList(m.m, o)
    return o


def mmatrixToMat44(mm):
    return Mat44([mm(0, 0), mm(0, 1), mm(0, 2), mm(0, 3),
                  mm(1, 0), mm(1, 1), mm(1, 2), mm(1, 3),
                  mm(2, 0), mm(2, 1), mm(2, 2), mm(2, 3),
                  mm(3, 0), mm(3, 1), mm(3, 2), mm(3, 3)])


def mayaNodeGetMat44(a):
    l = MSelectionList()
    MGlobal.getSelectionListByName(a, l)
    p = MDagPath()
    l.getDagPath(0, p)
    return mmatrixToMat44(p.inclusiveMatrix())


import contextlib


@contextlib.contextmanager
def mayaNodeContext():
    node = cmds.group(em=True)
    yield node
    cmds.delete(node)


def Mat44Identity():
    with mayaNodeContext() as node:
        m = mayaNodeGetMat44(node)
    return m


def Mat44Translate(x, y, z):
    with mayaNodeContext() as node:
        cmds.setAttr(node + '.translate', x, y, z, type='double3')
        m = mayaNodeGetMat44(node)
    return m

pi = 3.14159265358979323846
tau = pi + pi
rad2deg = 180.0 / pi
def Mat44RotateX(radians):
    with mayaNodeContext() as node:
        cmds.setAttr(node + '.rx', radians * rad2deg)
        m = mayaNodeGetMat44(node)
    return m


def Mat44RotateY(radians):
    with mayaNodeContext() as node:
        cmds.setAttr(node + '.ry', radians * rad2deg)
        m = mayaNodeGetMat44(node)
    return m


def Mat44RotateZ(radians):
    with mayaNodeContext() as node:
        cmds.setAttr(node + '.rz', radians * rad2deg)
        m = mayaNodeGetMat44(node)
    return m


def Mat44Rotate(radiansX, radiansY, radiansZ, rotateOrder):
    with mayaNodeContext() as node:
        cmds.setAttr(node + '.rotateOrder', rotateOrder)
        cmds.setAttr(node + '.rotate', radiansX * rad2deg, radiansY * rad2deg, radiansZ * rad2deg, type='double3')
        m = mayaNodeGetMat44(node)
    return m


def Mat44Rotate2(radians, rotateOrder):
    with mayaNodeContext() as node:
        cmds.setAttr(node + '.rotateOrder', rotateOrder)
        cmds.setAttr(node + '.rotate', radians[0] * rad2deg, radians[1] * rad2deg, radians[2] * rad2deg, type='double3')
        m = mayaNodeGetMat44(node)
    return m


def Mat44Scale(x, y, z):
    with mayaNodeContext() as node:
        cmds.setAttr(node + '.scale', x, y, z, type='double3')
        m = mayaNodeGetMat44(node)
    return m


def Mat44Scale2(f3):
    with mayaNodeContext() as node:
        cmds.setAttr(node + '.scale', *f3, type='double3')
        m = mayaNodeGetMat44(node)
    return m


def Mat44TranslateRotate(x, y, z, radiansX, radiansY, radiansZ, rotateOrder):
    with mayaNodeContext() as node:
        cmds.setAttr(node + '.rotateOrder', rotateOrder)
        cmds.setAttr(node + '.rotate', radiansX * rad2deg, radiansY * rad2deg, radiansZ * rad2deg, type='double3')
        cmds.setAttr(node + '.translate', x, y, z, type='double3')
        m = mayaNodeGetMat44(node)
    return m


def Mat44TranslateRotate2(translate, radians, rotateOrder):
    with mayaNodeContext() as node:
        cmds.setAttr(node + '.rotateOrder', rotateOrder)
        cmds.setAttr(node + '.rotate', radians[0] * rad2deg, radians[1] * rad2deg, radians[2] * rad2deg, type='double3')
        cmds.setAttr(node + '.translate', *translate, type='double3')
        m = mayaNodeGetMat44(node)
    return m


def Mat44TRS(x, y, z, radiansX, radiansY, radiansZ, sx, sy, sz, rotateOrder):
    with mayaNodeContext() as node:
        cmds.setAttr(node + '.rotateOrder', rotateOrder)
        cmds.setAttr(node + '.rotate', radiansX * rad2deg, radiansY * rad2deg, radiansZ * rad2deg, type='double3')
        cmds.setAttr(node + '.translate', x, y, z, type='double3')
        cmds.setAttr(node + '.scale', sx, sy, sz, type='double3')
        m = mayaNodeGetMat44(node)
    return m


def Mat44TRS2(t, r, s, o):
    with mayaNodeContext() as node:
        cmds.setAttr(node + '.rotateOrder', o)
        cmds.setAttr(node + '.rotate', r[0] * rad2deg, r[1] * rad2deg, r[2] * rad2deg, type='double3')
        cmds.setAttr(node + '.translate', *t, type='double3')
        cmds.setAttr(node + '.scale', *s, type='double3')
        m = mayaNodeGetMat44(node)
    return m


def Mat44Mul(am, bm):
    # am gets parented to bm, so we return am in the new space
    a, b = cmds.group(em=True), cmds.group(em=True)
    cmds.parent(a, b)
    cmds.xform(a, os=True, m=am)
    cmds.xform(b, os=True, m=bm)
    m = mayaNodeGetMat44(a)
    expected = mmatrixToMat44(mat44ToMMatrix(Mat44(am)) * mat44ToMMatrix(Mat44(bm)))
    assert str(m) == str(expected)
    cmds.delete(a, b)
    return m


def Mat44Inversed(m):
    with mayaNodeContext() as node:
        cmds.xform(node, ws=True, m=m)
        return Mat44(cmds.getAttr(node + '.worldInverseMatrix'))
    # mm = mat44ToMMatrix(Mat44(m))
    # mm = mm.inverse()
    # return mmatrixToMat44(mm)


Mat44InversedFast = Mat44Inversed
Mat44InversedFastNoScale = Mat44Inversed


def Mat44Transposed(m):
    mm = mat44ToMMatrix(Mat44(m))
    mm = mm.transpose()
    return mmatrixToMat44(mm)


def Mat44Determinant(m):
    mm = mat44ToMMatrix(Mat44(m))
    return mm.det4x4()


def Mat44VectorTransform(m, t):
    # t gets parented to m, so we return t in the new space
    a, b = cmds.group(em=True), cmds.group(em=True)
    cmds.parent(a, b)
    cmds.xform(a, os=True, m=[1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, t[0], t[1], t[2], 1])
    cmds.xform(b, os=True, m=m)
    m = mayaNodeGetMat44(a)
    cmds.delete(a, b)
    return m.col3.m128_f32[:-1]

# this is is synonymous with Mat44Mul
# def Mat44Parented(Mat44 child, Mat44 parent)

# the opposite of parenting
# so let's imagine we parented a to b and now we have a in world space
# and we want it back in local space when it were parented to b
# so we do a * b inverse = original a again
def Mat44Delta(am, bm):
    # we create the two objects in world space, parent a to b and return the object space matrix
    a, b = cmds.group(em=True), cmds.group(em=True)
    cmds.parent(a, b)
    cmds.xform(b, ws=True, m=bm)
    cmds.xform(a, ws=True, m=am)
    r = Mat44(cmds.xform(a, q=True, os=True, m=True))
    assert (r.m[3], r.m[7], r.m[11]) == (0.0, 0.0, 0.0)
    cmds.delete(a, b)
    return r


def Mat44FromVectors(c0, c1, c2, translate):
    return Mat44(c0 + c1 + c2 + translate)


def Mat44ToTop33(m):
    return Mat44([m[0], m[1], m[2], 0.0,
                  m[4], m[5], m[6], 0.0,
                  m[8], m[9], m[10], 0.0,
                  0.0, 0.0, 0.0, 1.0])


"""
TODO
def Mat44OrthoSymmetric(float width, float height, float near, float far):

These are not really supported in Maya land
def Mat44Frustum(float left, float right, float top, float bottom, float near, float far):
def Mat44PerspectiveY(float verticalFieldOfViewRadians, float aspectRatio, float near, float far):
def Mat44Orthographic(float left, float right, float top, float bottom, float near, float far):

def Mat44AxisAngle(axis, radians):
def Mat44Align(m128 from, m128 to):
def Mat44RotateTowards(m128 from, m128 to):
def Mat44LookAt(m128 targetDirection, m128 upDirection, EAxis forward, EAxis upAxis):
def Mat44ValidationFlags Mat44Validate(Mat44 m, Mat44ValidationFlags flags, float epsilon):
def Mat44MakeValid(Mat44 m, Mat44ValidationFlags flags):

"""


def randomValues(dimensions, counter, start, end, seed=0, cast=float):
    import random
    random.seed(seed)

    tmp = [0.0] * dimensions
    for i in xrange(counter):
        for j in xrange(dimensions):
            tmp[j] = cast(random.random() * (end - start) + start)
        yield tmp[:]


def timed(callable, inputs, repeat=7, star=False):
    import time
    import gc
    r = [None] * repeat
    for j in xrange(repeat):
        gcold = gc.isenabled()
        gc.disable()
        try:
            if star:
                t0 = time.clock()
                for v in inputs:
                    callable(*v)
                t1 = time.clock()
            else:
                t0 = time.clock()
                for v in inputs:
                    callable(v)
                t1 = time.clock()
        finally:
            if gcold:
                gc.enable()
        r[j] = (t1 - t0) / len(inputs)
    return min(r)


def run():
    # this is for our unit tests:
    # we build a json object that we can use to match our C++ results with later
    # making sure that our Maya implementation matches our C++ implementation
    from collections import OrderedDict
    inData = OrderedDict()
    outData = OrderedDict()
    randomVec3s = list(randomValues(3, 100, -1000.0, 1000.0, seed=0))
    randomFloats = list(randomValues(1, 100, -1000.0, 1000.0, seed=1))
    # append rotate order to random vec3s
    randomRotateOrders = list(randomValues(1, 100, 0.0, 5.9999, seed=2, cast=int))
    randomRotations = [randomVec3s[i] + j for (i, j) in enumerate(randomRotateOrders)]
    # same as randomRotations but grouped differently
    randomRotations2 = [((x, y, z), o) for (x, y, z, o) in randomRotations]
    # nested packing for vector arguments
    randomVec3s2 = [(l,) for l in randomVec3s]
    # TRS arguments
    s = list(randomValues(3, 100, -2.0, 2.0, seed=4))
    trs = [(t + randomVec3s[i] + s[i] + randomRotateOrders[i]) for i, t in enumerate(randomVec3s)]
    trs2 = [(t, randomVec3s[i], s[i], randomRotateOrders[i][0]) for i, t in enumerate(randomVec3s)]
    tr = [(t + randomVec3s[i] + randomRotateOrders[i]) for i, t in enumerate(randomVec3s)]
    tr2 = [(t, randomVec3s[i], randomRotateOrders[i][0]) for i, t in enumerate(randomVec3s)]
    
    outData['Mat44Identity'] = Mat44Identity().m

    def doOne(callable, randomInput):
        inData[callable.__name__] = randomInput
        outData[callable.__name__] = [None] * len(randomInput)
        for i, v in enumerate(randomInput):
            outData[callable.__name__][i] = callable(*v).m

    def doOnes(callable, randomInput):
        inData[callable.__name__] = randomInput
        outData[callable.__name__] = [None] * len(randomInput)
        for i, v in enumerate(randomInput):
            outData[callable.__name__][i] = callable(*v)

    doOne(Mat44Translate, randomVec3s)
    doOne(Mat44RotateX, randomFloats)
    doOne(Mat44RotateY, randomFloats)
    doOne(Mat44RotateZ, randomFloats)
    doOne(Mat44Rotate, randomRotations)
    doOne(Mat44Rotate2, randomRotations2)
    doOne(Mat44Scale, randomVec3s)
    doOne(Mat44Scale2, randomVec3s2)
    doOne(Mat44TranslateRotate, tr)
    doOne(Mat44TranslateRotate2, tr2)
    doOne(Mat44TRS, trs)
    doOne(Mat44TRS2, trs2)

    # assert outData['Mat44TRS'][0] == outData['Mat44TRS2'][0]
    m = outData['Mat44TRS']
    mult = [(m[i], m[i + 1]) for i in xrange(0, len(m), 2)]
    doOne(Mat44Mul, mult)

    mmm = [(m,) for m in outData['Mat44TRS']]
    doOne(Mat44Inversed, mmm)
    doOne(Mat44InversedFast, mmm)
    doOne(Mat44InversedFastNoScale, [(m,) for m in outData['Mat44TranslateRotate']])

    doOne(Mat44Transposed, mmm)
    doOnes(Mat44Determinant, mmm)
    doOnes(Mat44VectorTransform, [(m, randomVec3s[i]) for (i, m) in enumerate(outData['Mat44TRS'])])

    m = outData['Mat44TRS']
    delta = [(outData['Mat44Mul'][i / 2], m[i + 1]) for i in xrange(0, len(m), 2)]
    doOne(Mat44Delta, delta)

    inData['Mat44FromVectors'] = [(0.5, 0.0, 0.0, 0.0), (0.0, -1.0, 0.0, 0.0), (0.0, 0.0, 1.0, 0.0), (1.5, 2.0, 3.0, 1.0)]
    outData['Mat44FromVectors'] = Mat44FromVectors(*inData['Mat44FromVectors']).m

    doOne(Mat44ToTop33, [[m] for m in outData['Mat44TRS']])

    # cheat our way out of the decomposition by assuming we get the Mat44TRS matrix, 
    # decompose that, and match it with the trs inputs that were used
    inData['Mat44ToTranslate'] = outData['Mat44TRS']
    inData['Mat44ToEuler'] = [(m, randomRotateOrders[i]) for (i, m) in enumerate(outData['Mat44TRS'])]
    inData['Mat44ToScale'] = outData['Mat44TRS']
    outData['Mat44ToTranslate'] = randomVec3s
    outData['Mat44ToEuler'] = outData['Mat44Rotate'] # because there are multiple solutions we cast to euler and back and just compare matrices
    outData['Mat44ToScale'] = s

    # Perspective camera is hard, I've retrieved this through the maya API
    # but wnat to hard-code it because the default camera can change based on user prefs or last used settings
    aspect = 960.0 / 540.0
    fovx = (54.432223 / 180.0) * pi
    near = 0.1
    far = 10000.0
    perspX = [1.9444445371627808, 0.0, 0.0, 0.0,
        0.0, 3.4567902088165283, 0.0, 0.0, 
        0.0, 0.0, 1.000019907951355, -1.0, 
        0.0, 0.0, 0.20000198483467102, 0.0]
    # TODO: maya docs state that m22 and m32 are negated because 'maya uses a left handed coordinate system'
    # but actually comparing with the official glFrustum and gluPerspective docs these values are not negated at all.
    
    inData['Mat44PerspectiveX'] = [fovx, aspect, near, far]
    outData['Mat44PerspectiveX'] = perspX
    
    import json
    import os
    def dump(obj, fp):
        with open(fp, 'w') as fh:
            json.dump(obj, fh)

    outDir = os.path.abspath(__file__ + r'\\..')
    dump(inData, outDir + '\\in.json')
    dump(outData, outDir + '\\out.json')

"""
import maya_generate_json
reload(maya_generate_json)
maya_generate_json.run()

# extract perspective X matrix
# from maya.OpenMaya import *
# l=MSelectionList()
# MGlobal.getSelectionListByName('persp', l)
# p=MDagPath()
# l.getDagPath(0,p)
# m = MFnCamera(p).projectionMatrix()
# m(0,0), m(1,1), m(2,2), m(3,2), m(2,3)
# cmds.camera('persp', q=True, horizontalFieldOfView=True)
"""