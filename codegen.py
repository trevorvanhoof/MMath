# This utility regenerates a bunch of mmath.py, it is not fool proof so take caution with copying parts over.

import re
import os

pat = re.compile('DLL[ \t]*([a-zA-Z0-9_]+)[ \t]*([a-zA-Z0-9_]+)[ \t]*\((.*?)\)[ \t]*;', re.DOTALL | re.MULTILINE)

wrapperCode = []
dllCode = []

keys = ('Mat44', 'Quat', 'Vector', 'Friends')

for fname in keys:
    dllCode.append('    # %s.h' % fname)
    mat44 = os.path.join(os.path.abspath(__file__) + '/../MMath/%s.h' % fname)
    with open(mat44) as fh:
        code = fh.read()

    for match in pat.finditer(code):
        # if the first argument is not T, this is a static method
        funcName = match.group(2)
        # find second capital letter to get type name
        i = 1
        while funcName[i] not in r'ABCDEFGHIJKLMNOPQRSTUVWXYZ':
            i += 1
        T = funcName[:i]
        args = match.group(3).strip()
        if args:
            args = [a.strip().rsplit(' ', 1) for a in args.split(',')]
        else:
            args = []
        if not args or args[0][0] != T:
            wrapperCode.append('    @staticmethod')
        else:
            args[0][1] = 'self'
        argNames = ', '.join(a[1] for a in args)
        pyArgTypes = []
        for a in args:
            if a[0] == 'float':
                pyArgTypes.append('ctypes.c_float')
            else:
                pyArgTypes.append(a[0])

        if len(pyArgTypes) == 0:
            pyArgTypes = 'tuple()'
        elif len(pyArgTypes) == 1:
            pyArgTypes = '(%s,)' % pyArgTypes[0]
        else:
            pyArgTypes = '(%s)' % ', '.join(pyArgTypes)
        pyFuncName = funcName[len(T):]
        pyFuncName = pyFuncName[0].lower() + pyFuncName[1:]
        wrapperCode.append(
            '    def %s(%s):\n        return _dll().%s(%s)\n' % (pyFuncName, argNames, funcName, argNames))
        dllCode.append('    _instance.%s.argtypes = %s' % (funcName, pyArgTypes))
        dllCode.append('    _instance.%s.restype = %s' % (funcName, match.group(1)))

    print '\n'.join(wrapperCode).replace('__m128', 'Float4').replace('from', '_from')
    del wrapperCode[:]

print '\n'.join(dllCode).replace('__m128', 'Float4')