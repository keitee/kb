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
        # return self.data
        return 'superclass: data is {}'.format(self.data)

    def print_data(self):
        return 'superclass: data is {}'.format(self.data)

class SubClass(FirstClass):

    def display(self):
        return 'subclass: data is {}'.format(self.data)

# py-overload

class ThirdClass(SubClass):

    # ctor. ThirdClass(value). 
    # Q: possible to have other ctors?

    def __init__(self, value):
        print("ThirdClass: ctor: value {}".format(value))
        self.data = value

    # + operator.

    def __add__(self, other):
        print("ThirdClass: add: value {}".format(self.data + other.data))
        return ThirdClass(self.data + other.data)

    # print(instance) or instance.str()

    def __str__(self):
        print("ThirdClass: str: value {}".format(self.data))
        return "ThirdClass data {}".format(self.data)

    # this is function that intentionally wrong and not evaluated unless used.

    def xxx(self):
        print("ThirdClass: str: value {}" % (self.data))
        return xxx


class SampleClass:

    def __init__(self, value):
        print('SampleClass: ctor', value)
        self.data = value

class SubSampleClass(SampleClass):
    def get_value(self):
        return self.data

class TestClass(unittest.TestCase):

    def setUp(self):
        print('====================')
        print('[RUN] ', self._testMethodName)

    # [RUN]  test_class_ctor
    # SampleClass: ctor ('sample', 'class')
    # SampleClass: ctor subsample
    # subsample

    def test_class_ctor(self):

        # can take tuple and any
        so = SampleClass(('sample', 'class'))

        sso = SubSampleClass('subsample')
        print(sso.get_value())


    # name should start with "test_" and otherwise, will not be run.
    def test_class_attribute(self):

        # create two instances which has own namespace

        o1 = FirstClass()
        o2 = FirstClass()

        o1.set_data("King Arthur")
        o2.set_data(3.1459)

        self.assertEqual(o1.display(), 
                'superclass: data is King Arthur')
        self.assertEqual(o2.display(), 
                'superclass: data is 3.1459') 

        # no type, no access control

        o1.data = "Type Checking?"
        self.assertEqual(o1.display(), 
                'superclass: data is Type Checking?')

        o2.data = "Access Control?"
        self.assertEqual(o2.display(), 
                'superclass: data is Access Control?')

    def test_class_inheritance(self):

        o1 = SubClass()

        # *py-assign* *py-error-unbound*

        # if do not call set_data(), no `data` attribute is created.
        # since __init__() is not used

        # if we were to call display on one of our instances before calling
        # setdata, we would trigger an undefined name error—the attribute
        # named data doesn’t even exist in memory until it is assigned within
        # the setdata method.

        o1.set_data("SubClass")

        self.assertEqual(o1.display(), 
                'subclass: data is SubClass')

        # use it for free
        self.assertEqual(o1.print_data(), 
                'superclass: data is SubClass')


    # [RUN]  test_class_operator_overload
    # ThirdClass: ctor: value 100
    # ThirdClass: ctor: value 200
    # ThirdClass: add: value 300
    # ThirdClass: ctor: value 300
    # ThirdClass: str: value 300
    # ThirdClass data 300
    # ThirdClass: str: value 300
    # ThirdClass data 300

    def test_class_operator_overload(self):

        # py-error
        # TypeError: __init__() takes exactly 2 arguments (1 given)
        # o1 = ThirdClass()

        o1 = ThirdClass(100)
        o2 = ThirdClass(200)

        o3 = o1 + o2

        # both are same

        print(o3)
        print("{}".format(str(o3)))


#={===========================================================================
# py-class py-introspection

class TestClassIntrospection(unittest.TestCase):

    def setUp(self):
        print('====================')
        print('[RUN] ', self._testMethodName)

    def test_class_operator_overload(self):

        tc = ThirdClass(100)
        print(tc.__dict__)


#={===========================================================================
# py-override, CHAPTER 28 A More Realistic Example

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

        # *py-override*
        # Same result but bad way: cut and paste
        # The problem here is a very general one: anytime you copy code with cut
        # and paste, you essentially double your maintenance effort in the
        # future.
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


# ====================
# [RUN]  test_manager_class
# [Person: Tom Jones, None, 0]
# [Person: Ian King, 1000, 0]
# [Person: Pat King, 1000, manager]
# [Person: Susan King, manager, 1000]
#
# .====================
# [RUN]  test_preson_class
# [Person: Bob Smith, None, 0]
# [Person: Sue Jones, dev, 100000]
# Smith Jones
# [Person: Sue Jones, dev, 110000]
# == overrides ==
# Jones
# [Person: Tom Jones, mgr, 60000]
# == all three ==
# [Person: Bob Smith, None, 0]
# [Person: Sue Jones, dev, 121000]
# [Person: Tom Jones, mgr, 72000]

class TestPerson(unittest.TestCase):

    def setUp(self):
        print('====================')
        print('[RUN] ', self._testMethodName)
    
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

    # delegate all other undefined attrs in this class
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
        print('====================')
        print('[RUN] ', self._testMethodName)
    
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
        print('====================')
        print('[RUN] ', self._testMethodName)
    
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
        print('====================')
        print('[RUN] ', self._testMethodName)
    
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
        print('====================')
        print('[RUN] ', self._testMethodName)
    
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
# py-class-abc, CHAPTER 29 Class Coding Details
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
        print('====================')
        print('[RUN] ', self._testMethodName)
    
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
        print('====================')
        print('[RUN] ', self._testMethodName)
    
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
        print('====================')
        print('[RUN] ', self._testMethodName)
    
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

class Empty:

    # if has this, __getattr__() will be called
    # def __init__(self, value):
    #     self.age = value
    #     self.attrname = value

    # Here, the Empty class and its instance X have no real attributes of their
    # own, so the access to X.age gets routed to the __getattr__ method; self is
    # assigned the instance (X), and attrname is assigned the undefined
    # attribute name string ('age'). The class makes age look like a real
    # attribute by returning a real value as the result of the X.age
    # qualification expression (40). In effect, age becomes a dynamically
    # computed attribute

    def __getattr__(self, attrname):
        print('__getattr__ is called')
        if attrname == 'age':
            return 40
        else:
            raise AttributeError(attrname)

    # to avoid infinite recursion loop, use __dict__{}

    def __setattr__(self, attr, value):
        print('__setattr__ is called')
        if attr == 'age':
            self.__dict__[attr] = value + 10
        else:
            raise AttributeError(attr + ' not allowed')


class TestOverload(unittest.TestCase):

    def setUp(self):
        print('====================')
        print('[RUN] ', self._testMethodName)

    # [RUN] test_call_overload
    # ('Called:', (1, 2, 3), {})
    # ('Called:', (1, 2, 3), {'y': 5, 'x': 4})

    def test_overload_call_op(self):
        # C is callable
        C = Callee()
        C(1,2,3)
        C(1,2,3, x=4, y=5)

        x = Prod(2)
        self.assertEqual(x(3), 6)
        self.assertEqual(x(4), 8)

    def test_overload_getattr_op(self):

        # X = Empty(10)
        X = Empty()
        X.age
        X.age = 100
        with self.assertRaises(AttributeError):
            X.attrname

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
        print('====================')
        print('[RUN] ', self._testMethodName)

    def test_bound_callable_1(self):

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

    def test_bound_callable_2(self):
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
        print('====================')
        print('[RUN] ', self._testMethodName)

    def test_pattern_factory(self):
        object1 = factory(Spam)
        object2 = factory(Person, 'Arthur', 'King')
        object3 = factory(Person, name='Brian')

        self.assertEqual(object1.doit(99), 99)

        self.assertEqual(object2.name, 'Arthur')
        self.assertEqual(object2.job, 'King')

        self.assertEqual(object3.name, 'Brian')
        self.assertEqual(object3.job, None)


#={===========================================================================
# py-pattern-visitor 
# PYCB3, 8.21. Implementing the Visitor Pattern

class Node:
    pass

class UnaryOperator(Node):
    def __init__(self, operand):
        self.operand = operand

class BinaryOperator(Node):
    def __init__(self, left, right):
        self.left = left
        self.right = right

class AddNumber(BinaryOperator):
    pass

class SubNumber(BinaryOperator):
    pass

class MulNumber(BinaryOperator):
    pass

class DivNumber(BinaryOperator):
    pass

class NegateNumber(UnaryOperator):
    pass

class Number(Node):
    def __init__(self, value):
        self.value = value

class NodeVisitor:

    def visit(self, node):

        # get method
        methname = 'visit_' + type(node).__name__
        meth = getattr(self, methname, None)
        if meth is None:
            meth = self.generic_visit
        return meth(node)

    def generic_visit(self, node):
        raise RuntimeError(
                'No {} method'.format('visit_' + type(node).__name__)
                )

class Evaluator(NodeVisitor):

    def visit_SubNumber(self, node):
        return self.visit(node.left) - self.visit(node.right)

    def visit_AddNumber(self, node):
        return self.visit(node.left) + self.visit(node.right)

    def visit_MulNumber(self, node):
        return self.visit(node.left) * self.visit(node.right)

    def visit_DivNumber(self, node):
        return self.visit(node.left) / self.visit(node.right)

    def visit_Number(self, node):
        return node.value


class TestPatternVisitor(unittest.TestCase):

    def setUp(self):
        print('====================')
        print('[RUN] ', self._testMethodName)

    def test_pattern_visitor(self):
        t1 = SubNumber(Number(3), Number(4))
        t2 = MulNumber(Number(2), t1)
        t3 = DivNumber(t2, Number(5))
        t4 = AddNumber(Number(1), t3)

        e = Evaluator()
        print(e.visit(t1))
        print(e.visit(t2))
        print(e.visit(t3))
        print(e.visit(t4))


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
        print('id: %d' % StaticClass.track_id)


class TestStaticClass_1(unittest.TestCase):

    def setUp(self):
        print('====================')
        print('[RUN] ', self._testMethodName)

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
    print('id: %d' % StaticClass.track_id)

class StaticClass:
    track_id = 0

    def __init__(self):
        StaticClass.track_id += 1

class TestStaticClass_2(unittest.TestCase):

    def setUp(self):
        print('====================')
        print('[RUN] ', self._testMethodName)

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
        print('====================')
        print('[RUN] ', self._testMethodName)

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
        print('====================')
        print('[RUN] ', self._testMethodName)

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
        print('====================')
        print('[RUN] ', self._testMethodName)

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
        print('====================')
        print('[RUN] ', self._testMethodName)

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
        print('====================')
        print('[RUN] ', self._testMethodName)

    # [RUN] test_decorator_user_defined
    # call 1 to spam
    # 6
    # call 2 to spam
    # abc

    def test_decorator_user_defined(self):
        print(spam(1, 2, 3))
        print(spam('a', 'b', 'c'))


#={===========================================================================
# py-property

class PersonProperty:
    def __init__(self, name):
        self._name = name

    def getName(self):
        print('fetch...')
        return self._name

    def setName(self, value):
        print('change...')
        self._name = value

    def delName(self):
        print('remove...')
        del self._name

    name = property(getName, setName, delName, 'name property docs')

class PersonPropertyDecorator:
    def __init__(self, name):
        self._name = name

    # name = property(name)
    @property
    def name(self):
        "name property docs"
        print('fetch...')
        return self._name

    # name = name.setter(name)
    @name.setter
    def name(self, value):
        print('change...')
        self._name = value

    # name = name.deleter(name)
    @name.deleter
    def name(self):
        print('remove...')
        del self._name

    # name = property(getName, setName, delName, 'name property docs')


# PYCB3, 8.6. Creating Managed Attributes

class PersonCookbook_8_6:

    def __init__(self, first_name):
        self.first_name = first_name

    @property
    def first_name(self):
        return self._first_name

    @first_name.setter
    def first_name(self, value):
        if not isinstance(value, str):
            raise TypeError('Expected a string')
        self._first_name = value

    @first_name.deleter
    def first_name(self):
        raise AttributeError("can't delete attribute")

class TestDecoratorProperty(unittest.TestCase):

    def setUp(self):
        print('====================')
        print('[RUN] ', self._testMethodName)

    # [RUN]  test_decorator_property
    # fetch...
    # Bob Smith
    # change...
    # fetch...
    # Robert Smith
    # remove...
    # fetch...
    # Sue Jones
    # name property docs

    def test_decorator_property(self):
        bob = PersonProperty('Bob Smith')
        print(bob.name)

        bob.name = 'Robert Smith'
        print(bob.name)

        del bob.name

        # AttributeError: 'PersonProperty' object has no attribute '_name'
        # print(bob.name)

        sue = PersonProperty('Sue Jones')
        print(sue.name)
        print(PersonProperty.name.__doc__)

    def test_decorator_property_decorator(self):
        bob = PersonPropertyDecorator('Bob Smith')
        print(bob.name)

        bob.name = 'Robert Smith'
        print(bob.name)

        del bob.name

        # AttributeError: 'PersonProperty' object has no attribute '_name'
        # print(bob.name)

        sue = PersonPropertyDecorator('Sue Jones')
        print(sue.name)
        print(PersonPropertyDecorator.name.__doc__)

    def test_decorator_cookbook_8_3(self):
        a = PersonCookbook_8_6('Guido')
        print(a.first_name)

        with self.assertRaises(TypeError):
            a.first_name = 42

        with self.assertRaises(AttributeError):
            del a.first_name 


#={===========================================================================
# py-descriptor

class Name:
    "name descriptor docs"
    def __get__(self, instance, owner):
        print('fetch...')
        return instance._name

    def __set__(self, instance, value):
        print('change...')
        instance._name = value
    
    def __delete__(self, instance):
        print('remove...')
        del instance._name

class PersonDescriptor:
    def __init__(self, name):
        self._name = name

    # assign descriptor to attr
    name = Name()


# PYCB3, 8.9. Creating a New Kind of Class or Instance Attribute

class Integer:
    def __init__(self, name):
        self.name = name

    def __get__(self, instance, cls):
        if instance is None:
            return self
        else:
            # return instance.x or return instance.y?
            # use Integer(str) and str is attribute name.
            return instance.__dict__[self.name]

    def __set__(self, instance, value):
        if not isinstance(value, int):
            raise TypeError('Expected an int')
        instance.__dict__[self.name] = value

    def __delete__(self, instance):
        del instance.__dict__[self.name]

class Point:

    # py-class-variable
    x = Integer('x')
    y = Integer('y')

    def __init__(self, x, y):
        self.x = x
        self.y = y

class TestDescriptor(unittest.TestCase):

    def setUp(self):
        print('====================')
        print('[RUN] ', self._testMethodName)

    # [RUN]  test_descriptor
    # fetch...
    # Bob Smith
    # change...
    # fetch...
    # Robert Smith
    # remove...
    # fetch...
    # Sue Jones
    # name property docs

    def test_descriptor(self):

        # as `int type`
        xxx = PersonProperty(100)
        self.assertEqual(type(xxx.name), type(int()))

        # as `str type`
        bob = PersonProperty('Bob Smith')
        self.assertEqual(type(bob.name), type(str()))
        print(bob.name)

        bob.name = 'Robert Smith'
        print(bob.name)

        del bob.name

        sue = PersonProperty('Sue Jones')
        print(sue.name)
        print(PersonProperty.name.__doc__)

    def test_cookbook_8_9(self):

        p = Point(2, 3)
        self.assertEqual(p.x, 2)
        p.y = 5

        with self.assertRaises(TypeError):
            p.x = 2.3


#={===========================================================================
if __name__ == '__main__':
    unittest.main()
