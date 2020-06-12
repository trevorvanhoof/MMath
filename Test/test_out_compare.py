import json
import os
from collections import OrderedDict

with open(os.path.join(os.path.abspath(__file__), '../in.json')) as fh:
    input = json.load(fh, object_pairs_hook=OrderedDict)
with open(os.path.join(os.path.abspath(__file__), '../out.json')) as fh:
    maya = json.load(fh, object_pairs_hook=OrderedDict)
with open(os.path.join(os.path.abspath(__file__), '../mmath_out.json')) as fh:
    mmath = json.load(fh, object_pairs_hook=OrderedDict)

assert set(maya.keys()) == set(mmath.keys())  # ensure both entrypoints provide the same set of tests


def floatArrayCompare(a, b, *errorInfo):
    eps = 0.0002  # this was tweaked because of by far the largest error occuring in Mat44Delta

    assert len(a) == len(b), (a, b)
    err = 0.0
    for i, e in enumerate(a):
        assert isinstance(e, float)
        assert isinstance(b[i], float)
        err += abs(b[i] - e)
    err /= float(len(a))

    if err > eps:
        print errorInfo + (err,)
        if len(a) == 16:
            print '%.03f %.03f %.03f %.03f\n%.03f %.03f %.03f %.03f\n%.03f %.03f %.03f %.03f\n%.03f %.03f %.03f %.03f' % tuple(a)
            print '%.03f %.03f %.03f %.03f\n%.03f %.03f %.03f %.03f\n%.03f %.03f %.03f %.03f\n%.03f %.03f %.03f %.03f' % tuple(b)
        elif len(a) == 3:
            print '%.03f %.03f %.03f' % tuple(a)
            print '%.03f %.03f %.03f' % tuple(b)
            # else:
            print a
            print b
        # raise RuntimeError(err)


for key in maya:
    print key
    if isinstance(maya[key][0], list):
        for i, item in enumerate(maya[key]):
            floatArrayCompare(item, mmath[key][i], i, input[key][i])
    else:  # TODO: This probably messes with the determinant?
        floatArrayCompare(maya[key], mmath[key])
