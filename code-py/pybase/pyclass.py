import unittest

# $ python coll_test.py --verbose
#
# to run single test
# $ python -m unittest coll_test.TestDictCreation.test_create_by_assign
# 
# to run all of a class
# $ python -m unittest coll_test.TestDictCreation

#={===========================================================================
# py-class

class FirstClass:

    def set_data(self, value):
        self.data = value

    def display(self):
        # print(self.data)
        return self.data

    def print_data(self):
        print "superclass: data is {}".format(self.data)

class SubClass(FirstClass):

    def display(self):
        print "subclass: data is {}".format(self.data)
        return self.data

#={===========================================================================
class TestClass(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN] ", self._testMethodName

    # name should start with "test_" and otherwise, will not be run.
    def test_attribute(self):

        # create two instances which has own namespace

        o1 = FirstClass()
        o2 = FirstClass()

        o1.set_data("King Arthur")
        o2.set_data(3.1459)

        self.assertEqual(o1.display(), "King Arthur")
        self.assertEqual(o2.display(), 3.1459)


        # no type, no access control

        o1.data = "Type Checking?"
        self.assertEqual(o1.display(), "Type Checking?")

        o2.data = "Access Control?"
        self.assertEqual(o2.display(), "Access Control?")


class TestClassInheritance(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN] ", self._testMethodName

    def test_inheritance(self):

        o1 = SubClass()

        # if do not call set_data(), no `data` attribute is created.
        o1.set_data("SubClass")

        self.assertEqual(o1.display(), "SubClass")

        # prints "SubClass"
        o1.print_data()


class ThirdClass(SubClass):

    # ctor. ThirdClass(value). 
    # Q: possible to have other ctors?

    def __init__(self, value):
        print "ThirdClass: ctor: value {}".format(value)
        self.data = value

    # + operator.

    def __add__(self, other):
        print "ThirdClass: add: value {}".format(self.data + other.data)
        return ThirdClass(self.data + other.data)

    # print(instance) or instance.str()

    def __str__(self):
        print "ThirdClass: str: value {}".format(self.data)
        return "ThirdClass data {}".format(self.data)

    # this is function that intentionally wrong and not evaluated unless used.

    def xxx(self):
        print "ThirdClass: str: value {}" % (self.data)
        return xxx

class TestClassOperatorOverload(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN] ", self._testMethodName

    def test_operator_overload(self):

        # py-error
        # TypeError: __init__() takes exactly 2 arguments (1 given)
        # o1 = ThirdClass()

        o1 = ThirdClass(100)
        o2 = ThirdClass(200)

        o3 = o1 + o2

        # both are same

        print(o3)
        print "{}".format(str(o3))


#={===========================================================================
# CHAPTER 28 A More Realistic Example

class Person:

    # *py-default-argument*
    # As with C++, any arguments in a function header after the first default
    # must all have defaults, too:

    def __init__(self, name, job=None, pay=0):
        self.name = name
        self.job = job
        self.pay = pay

    def lastName(self):
        return self.name.split()[-1]

    def giveRaise(self, percent):
        self.pay = int(self.pay * (1 + percent))

    def __repr__(self):
        return '[Person: %s, %s, %s]' % (self.name, self.job, self.pay)

class Manager(Person):

        # because Manager had no __init__ constructor, it inherits that
        # in Person.

    def giveRaise(self, percent, bonus=.10):
        # Bad: cut and paste
        # self.pay = int(self.pay * (1 + percent + bonus)) 

        # *py-class-call-base-version*
        # Good: augment original
        Person.giveRaise(self, percent + bonus)


class ManagerImproved(Person):

    # redefine ctor
    def __init__(self, name, pay):
        Person.__init__(self, name, 'manager', pay)

    def giveRaise(self, percent, bonus=.10):
        # Bad: cut and paste
        # self.pay = int(self.pay * (1 + percent + bonus)) 

        # *py-class-call-base-version*
        # Good: augment original
        Person.giveRaise(self, percent + bonus)

class TestPerson(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName
    
    def test_preson_class(self):
        bob = Person('Bob Smith')
        sue = Person('Sue Jones', job='dev', pay=100000)
        print(bob)
        print(sue)
        print(bob.lastName(), sue.lastName())
        sue.giveRaise(.10)
        print(sue)

        #
        print("== overrides ==")
        tom = Manager('Tom Jones', 'mgr', 50000)
        tom.giveRaise(.10)
        print(tom.lastName())
        print(tom)

        # py-class-polymorphism
        print("== all three ==")
        for obj in (bob, sue, tom):
            obj.giveRaise(.10)
            print(obj)

    def test_manager_class(self):
        tom = Manager('Tom Jones')
        print(tom)

        ian = Manager('Ian King', 1000)
        print(ian)

        pat = Manager('Pat King', 1000, 'manager')
        print(pat)

        sus = ManagerImproved('Susan King', 1000)
        print(sus)


class CompositeManager:

    # has a person
    def __init__(self, name, pay):
        self.person = Person(name, 'manager', pay)

    # forwards instead
    def giveRaise(self, percent, bonus=.10):
        # Person.giveRaise(self, percent + bonus)
        self.person.giveRaise(percent + bonus)

    # delegate all other attrs
    # *py-getattr*
    # since this class do not use inheritance, all attribute fetch request comes
    # to this and forward them to the composite.

    def __getattr__(self, attr):
        return getattr(self.person, attr)

    # must overload in 3.x and 2.x is okay with/without this
    # (in 3.X, at least, as noted in the upcoming sidebar "Catching Built-in
    # Attributes in 3.X" on page 839);

    def __repr__(self):
        return str(self.person)

class TestComposite_1(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName
    
    def test_composite_manager_class(self):
        tom = Manager('Tom Jones')
        print(tom)

        ian = Manager('Ian King', 1000)
        print(ian)

        pat = Manager('Pat King', 1000, 'manager')
        print(pat)

        sus = ManagerImproved('Susan King', 1000)
        print(sus)

class Department:
    # causes an runtime error
    def __init__(self, *args):
        self.members = list(args)

    def addMember(self, person):
        self.members.append(person)

    def giveRaises(self, percent):
        for person in self.members:
            person.giveRaise(percent)

    def showAll(self):
        for person in self.members:
            print(person)

class TestComposite_2(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName
    
    # [RUN] test_department_class
    # [Person: Bob Smith, None, 0]
    # [Person: Sue Jones, dev, 11000]
    # [Person: Tom Jones, 50000, 0]

    def test_department_class(self):
        bob = Person('Bob Smith')
        sus = Person('Sue Jones', job='dev', pay=10000)
        tom = Manager('Tom Jones', 50000)

        development = Department(bob, sus)
        development.addMember(tom)
        development.giveRaises(.10)
        development.showAll()


#={===========================================================================
# py-class-attribute
# "Assorted class utilities and tools"

class AttrDisplay:
    """
    Provides an inheritable display overload method that shows
    instances with their class names and a name=value pair for
    each attribute stored on the instance itself (but not attrs
    inherited from its classes). Can be mixed into any class,
    and will work on any instance.
    """
    def gatherAttrs(self):
        # sort attrs and make a string from these
        attrs = []
        for key in sorted(self.__dict__):
            attrs.append('%s = %s ' % (key, getattr(self, key)))
        return ','.join(attrs)

    def __repr__(self):
        return '[%s: %s]' % (self.__class__.__name__, self.gatherAttrs())

class TopTest(AttrDisplay):
    count = 0

    # *py-error*
    # no error message from python and simply wrong result. spend time to figure
    # out and the thing is no ctor is called; not overrided. 
    #
    # def __int__(self):

    def __init__(self):
        self.attr1 = TopTest.count
        self.attr2 = TopTest.count+1
        TopTest.count += 2

class SubTest(TopTest):
    pass

class TestAttrDisplay(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName
    
    # [RUN] test_attr_display
    # [TopTest: attr1 = 0,attr2 = 1]
    # [SubTest: attr1 = 2,attr2 = 3]

    def test_attr_display(self):
        X, Y = TopTest(), SubTest()
        print(X)
        print(Y)


#={===========================================================================
# py-db

"""
Storing Objects on a Shelve Database

kyoupark@kit-debian64:~/git/kb/code-py/pyclass$ file persondb 
persondb: Berkeley DB (Hash, version 9, native byte-order)

kyoupark@kit-debian64:~/git/kb/code-py/pyclass$ python
Python 2.7.9 (default, Jun 29 2016, 13:08:31) 
[GCC 4.9.2] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> import shelve
>>> db = shelve.open('persondb')
>>> len(db)
3
>>> list(db.keys())
['Bob Smith', 'Sue Jones', 'Tom Jones']
>>> bob = db['Bob Smith']
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
  File "/usr/lib/python2.7/shelve.py", line 122, in __getitem__
    value = Unpickler(f).load()
AttributeError: 'module' object has no attribute 'Person'
>>> 

Notice that we don't have to import our Person or Manager classes here in order
to load or use our stored objects. For example, we can call bob's lastName
method freely, and get his custom print display format automatically, even
though we don't have his Person class in our scope here. This works because when
Python pickles a class instance, it records its self instance attributes, along
with the name of the class it was created from and the module where the class
lives. When bob is later fetched from the shelve and unpickled, Python will
automatically reimport the class and link bob to it.

Have tried to add the full path of current directory, to have Person.py, and to
import Person in the interactive. None of these works. WHY?


"""
class TestDatabase(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName
    
    def test_shelve(self):
        bob = Person('Bob Smith')

        sue = Person('Sue Jones', job='dev', pay=100000)
        sue.giveRaise(.10)

        tom = Manager('Tom Jones', 'mgr', 50000)
        tom.giveRaise(.10)

        # py-class-polymorphism
        for obj in (bob, sue, tom):
            obj.giveRaise(.10)
            print(obj)

        print("== storing objects ==")

        import shelve
        db = shelve.open('persondb')
        for obj in (bob, sue, tom):
            db[obj.name] = obj

        db.close()


#={===========================================================================
# CHAPTER 29 Class Coding Details

# Class Interface Techniques

class Super:
    def method(self):
        print('in Super.method')

    def delegate(self):
        # expected to be defined
        self.action()

# Implements the action method expected by Super's delegate method.
class Provider(Super):
    def action(self):
        print('in Provider.action')

# [RUN] test_abstract_method
# in Provider.action

class TestAbstract(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName
    
    def test_abstract_method(self):
        x = Provider()
        x.delegate()
        
# in Python 2.6 and 2.7, we use a class attribute instead:
from abc import ABCMeta, abstractmethod

class ABCSuper:
    __metaclass__ = ABCMeta
    @abstractmethod
    def method(self):
        pass

class ABCSub(ABCSuper):
    pass

class TestAbstractCreation(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName
    
    def test_abstract_super_creation(self):
        x = ABCSuper()

    def test_abstract_sub_creation(self):
        x = ABCSub()


#={===========================================================================
# py-override
# Chapter 31: Designing with Classes
# Stream Processors Revisited

class Processor:
    def __init__(self, reader, writer):
        self.reader = reader
        self.writer = writer

    def process(self):
        while True:
            data = self.reader.readline()
            if not data: break
            data = self.converter(data)
            self.writer.write(data)

    def converter(self, data):
        assert False, 'converter must be defined'

class Uppercase(Processor):
    def converter(self, data):
        return data.upper()

class HTMLize:
    def write(self, line):
        print('<PRE>%s</PRE>' % line.rstrip())

# [RUN] test_processor
# SPAM
# SPAM
# SPAM!
#
# kyoupark@kit-debian64:~/git/kb/code-py/pyclass$ more trispamup.txt 
# SPAM
# SPAM
# SPAM!
# 
# [RUN] test_htmlize_stream
# <PRE>SPAM</PRE>
# <PRE>SPAM</PRE>
# <PRE>SPAM!</PRE>

# If you trace through this example's control flow, you'll see that we get both
# uppercase conversion (by inheritance) and HTML formatting (by composition),

class TestProcessor(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName
    
    def test_processor(self):
        import sys
        obj = Uppercase(open('trispam.txt'), sys.stdout)
        obj.process()

    # To process different sorts of streams, pass in different sorts of objects
    def test_different_stream(self):
        import sys
        obj = Uppercase(open('trispam.txt'), open('trispamup.txt', 'w'))
        obj.process()

    # To process different sorts of streams, pass in different sorts of objects
    def test_htmlize_stream(self):
        import sys
        obj = Uppercase(open('trispam.txt'), HTMLize())
        obj.process()


#={===========================================================================
# py-overload 
# Chapter 30: Operator Overloading

# py-callable
class Callee:

    # py-keyward-argument
    def __call__(self, *pargs, **kargs):
        # accept arbitrary arguments
        print('Called:', pargs, kargs)

class Prod:
    def __init__(self, value):
        self.value = value

    def __call__(self, other):
        return self.value * other

class TestOverload(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName

    # [RUN] test_call_overload
    # ('Called:', (1, 2, 3), {})
    # ('Called:', (1, 2, 3), {'y': 5, 'x': 4})

    def test_call_overload(self):
        # C is callable
        C = Callee()
        C(1,2,3)
        C(1,2,3, x=4, y=5)

    def test_callable(self):
        x = Prod(2)
        self.assertEqual(x(3), 6)
        self.assertEqual(x(4), 8)

class Spam:
    def doit(self, message):
        return message

class Number:
    def __init__(self, base):
        self.base = base

    def double(self):
        return self.base * 2

    def triple(self):
        return self.base * 3

class TestBoundMethod(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName

    def test_callable_1(self):

        object1 = Spam()
        self.assertEqual(object1.doit('hello world'), 'hello world')

        # bound
        object2 = Spam()
        # x is bound method object
        x = object2.doit
        self.assertEqual(x('hello world'), 'hello world')

        # unbound
        object3 = Spam()
        t = Spam.doit
        self.assertEqual(t(object3, 'howdy'), 'howdy')
        
    # [RUN] test_callable_2
    # 4
    # 6
    # 9
    # 8

    def test_callable_2(self):
        x = Number(2)
        y = Number(3)
        z = Number(4)

        # list of bound objects
        acts = [x.double, y.double, y.triple, z.double]

        for act in acts:
            print(act())


#={===========================================================================
# py-pattern-facory 
# Chapter 31: Designing with Classes
# Classes Are Objects: Generic Object Factories

# The function uses special "varargs" call syntax to call the function and
# return an instance.

def factory(aClass, *pargs, **kargs):
    return aClass(*pargs, **kargs)

class TestPatternFactory(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName

    def test_factory(self):
        object1 = factory(Spam)
        object2 = factory(Person, 'Arthur', 'King')
        object3 = factory(Person, name='Brian')

        self.assertEqual(object1.doit(99), 99)

        self.assertEqual(object2.name, 'Arthur')
        self.assertEqual(object2.job, 'King')

        self.assertEqual(object3.name, 'Brian')
        self.assertEqual(object3.job, None)


#={===========================================================================
# py-class-static

# tries to mimic c++ cookbook, 8.4 Automatically Adding New Class Instances to a
# Container

class StaticClass:
    track_id = 0

    def __init__(self):
        StaticClass.track_id += 1

    # selfless method
    def show_id():
        print 'id: %d' % StaticClass.track_id


class TestStaticClass_1(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName

    def test_static_class_in_py2x(self):

        # TypeError: unbound method show_id() must be called with StaticClass
        # instance as first argument (got nothing instead)

        # this is okay in *py-3x*
        # StaticClass.show_id()

        with self.assertRaises(TypeError):
            StaticClass.show_id()

        # TypeError: show_id() takes no arguments (1 given)

        sc = StaticClass()
        with self.assertRaises(TypeError):
            sc.show_id()


# use simple method and works for both version

def show_id():
    print 'id: %d' % StaticClass.track_id

class StaticClass:
    track_id = 0

    def __init__(self):
        StaticClass.track_id += 1

class TestStaticClass_2(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName

    def test_static_class_use_simple_method(self):

        sc = StaticClass()
        show_id()
        print(StaticClass.track_id)


class Methods:
    # normal instance method
    def imeth(self, x):
        print([self, x])

    # static method
    def smeth(x):
        print([x])

    # class method
    def cmeth(cls, x):
        print([cls, x])

    # Notice how the last two assignments in this code simply reassign
    # (a.k.a. rebind) the method names smeth and cmeth.

    smeth = staticmethod(smeth)
    cmeth = classmethod(cmeth)


class TestStaticClass_3(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName

    # [RUN] test_static_class_use_staticmethod
    # [<pyclass.Methods instance at 0x7efd73c7d1b8>, 1]
    # [<pyclass.Methods instance at 0x7efd73c7d1b8>, 2]
    # [3]
    # [4]
    # [<class pyclass.Methods at 0x7efd73c67f58>, 5]
    # [<class pyclass.Methods at 0x7efd73c67f58>, 6]

    def test_static_class_use_staticmethod(self):

        m = Methods()
        m.imeth(1)
        Methods.imeth(m, 2)

        # now worlks for *py-2x*
        Methods.smeth(3)
        m.smeth(4)

        # Class methods are similar, but Python automatically passes the class
        # (not an instance) in to a class method's first (leftmost) argument,
        # whether it is called through a class or an instance:

        # Class method: call through class
        # Becomes cmeth(Methods, 5)

        Methods.cmeth(5)

        # Class method: call through instance
        # Becomes cmeth(Methods, 5)

        m.cmeth(6)


class Spam:

    numInstances = 0

    def __init__(self):
        Spam.numInstances += 1

    def printNumInstances():
        print('number of instances: %s' % Spam.numInstances)

    printNumInstances = staticmethod(printNumInstances)

class TestStaticClass_4(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName

    # [RUN] test_static_class_use_staticmethod_2
    # number of instances: 2
    # number of instances: 2

    def test_static_class_use_staticmethod_2(self):
        a = Spam()
        b = Spam()
        Spam.printNumInstances()
        a.printNumInstances()

class Sub(Spam):

    def printNumInstances():
        print('extra stuff...')
        Spam.printNumInstances()

    printNumInstances = staticmethod(printNumInstances)

class TestStaticClass_5(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName

    # [RUN] test_static_class_use_staticmethod_2
    # extra stuff...
    # number of instances: 2
    # number of instances: 2
    # extra stuff...
    # number of instances: 2

    def test_static_class_use_staticmethod_2(self):
        a = Sub()
        b = Sub()
        Sub.printNumInstances()
        Spam.printNumInstances()
        a.printNumInstances()


#={===========================================================================
# py-decorator

class DecoratedSpam:

    numInstances = 0

    def __init__(self):
        Spam.numInstances += 1

    @staticmethod
    def printNumInstances():
        print('number of instances: %s' % Spam.numInstances)


class TestStaticClass_6(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName

    # [RUN] test_static_class_use_staticmethod_2
    # number of instances: 2
    # number of instances: 2

    def test_static_class_use_staticmethod_2(self):
        a = DecoratedSpam()
        b = DecoratedSpam()
        DecoratedSpam.printNumInstances()
        a.printNumInstances()


# LPY5, Decorators and Metaclasses: Part 1
# A First Look at User-Defined Function Decorators

class tracer:
    def __init__(self, func):
        self.calls = 0
        self.func = func

    # add logic and run original

    def __call__(self, *args):
        self.calls += 1
        print('call %s to %s' % (self.calls, self.func.__name__))
        return self.func(*args)

@tracer
def spam(a, b, c):
    return a + b + c

class TestDecorator_1(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN]", self._testMethodName

    # [RUN] test_decorator_user_defined
    # call 1 to spam
    # 6
    # call 2 to spam
    # abc

    def test_decorator_user_defined(self):
        print(spam(1, 2, 3))
        print(spam('a', 'b', 'c'))


#={===========================================================================
if __name__ == '__main__':
    unittest.main()
