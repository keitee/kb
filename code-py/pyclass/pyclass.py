import unittest

# $ python coll_test.py --verbose
#
# to run single test
# $ python -m unittest coll_test.TestDictCreation.test_create_by_assign
# 
# to run all of a class
# $ python -m unittest coll_test.TestDictCreation

#={===========================================================================
# pyclass

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

"""
class Department:
    # causes an runtime error
    def __int__(self, *args):
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
    
    def test_department_class(self):
        bob = Person('Bob Smith')
        sus = Person('Sue Jones', job='dev', pay=10000)
        tom = Manager('Tom Jones', 50000)

        development = Department(bob, sus)
        development.addMember(tom)
        development.giveRaises(.10)
        development.showAll()
"""

## "Assorted class utilities and tools"

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
    
    def test_attr_display(self):
        X, Y = TopTest(), SubTest()
        print(X)
        print(Y)

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
if __name__ == '__main__':
    unittest.main()
