"""
Enum recipe.

Consider this enum:

class EItemType(Enum, int):
    Character = 0

Global members like 'Character' are converted to an instance of the enum subclass.
The value is asserted isinstance() of the second base class (OPTIONAL).
This way enum members retain their value type (because EItemType is a subclass of int)
but are also more narrowly typed to the specific enum they belong to.

__str__ is overridden to print value <class.name>:
print EItemType.Character # 0 <EItemType.Character>

__contains__ is overridden (on the type!), it works with instances and values
(so does not use instancecheck but a lookup table)
print EItemType.Character in EItemType # True
print 0 in EItemType # True

__instancecheck__ works out of the box because we wrap the values already:
print isinstance(EItemType.Character, int) # True
print isinstance(EItemType.Character, EItemType) # True

Last we can look up labels from values
print EItemType.name(EItemType.Character) # 'Character'
print EItemType.name(0) # 'Character'

# For convenience we also added a member to the Enum instances which calls the name() method above:
print EItemType.Character.name() # 'Character'
"""
from __future__ import absolute_import, print_function
from collections import OrderedDict


class EnumMeta(type):
    def __new__(mcls, name, bases, namespace):
        if name == 'Enum':
            return super(EnumMeta, mcls).__new__(mcls, name, bases, namespace)
        assert bases[0] == Enum
        if len(bases) == 1:
            bases = bases[0], object
        assert len(bases) == 2
        valueConstraint = None if len(bases) == 1 else bases[1]
        cls = super(EnumMeta, mcls).__new__(mcls, name, bases, namespace)
        cls._enum = OrderedDict()
        cls._inverse = OrderedDict()
        for key, value in cls.__dict__.items():
            if key.startswith('_'):
                continue
            if not isinstance(value, classmethod):
                if valueConstraint is not None:
                    assert isinstance(value, valueConstraint), value
                valueInstance = cls(value)
                cls._enum[key] = valueInstance
                cls._inverse[value] = key
                cls._inverse[valueInstance] = key
            else:
                valueInstance = value
            setattr(cls, key, valueInstance)
        return cls

    def __contains__(cls, value):
        return value in cls._inverse

    def name(cls, value):
        return cls._inverse[value]

    def iteritems(cls):
        return cls._enum.items()


class Enum(object, metaclass=EnumMeta):
    def __init__(self, value):
        assert not isinstance(value, Enum)
        self.value = value

    def __str__(self):
        return '%s <%s.%s>' % (self.value, self.__class__.__name__, self.__class__._inverse[self.value])

    def name(self):
        return self.__class__._inverse[self.value]
