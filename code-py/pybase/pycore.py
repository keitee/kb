import unittest
import subprocess

"""
shall use python3

$ python3 coll_test.py --verbose

1. to run single test
$ python -m unittest coll_test.TestDictCreation.test_create_by_assign

   to run single test set
$ python3 -m unittest pycore.TestList

2. to run all of a class
$ python -m unittest coll_test.TestDictCreation

3. must have unique class name or it will not run without error

class TestAttribute(unittest.TestCase):

4. has setUp() to see which test gets run
"""

#={===========================================================================
# py-attribute

class TestAttribute(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)


    # [RUN]  test_attribute
    # <type 'str'>
    # <built-in method split of str object at 0x7fa77a489a80>

    def test_attribute(self):

        a = 'foo'
        print(type(a))
        print(getattr(a, 'split'))


#={===========================================================================
# py-type

class TestType(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_reference_semantic(self):
        a = [1,2,3,4]
        b = a
        a.append(5)
        self.assertEqual(b, [1,2,3,4,5])

    def test_mutable(self):

        # no shared reference since they are immutable
        a = 3
        b = a
        a = a + 2
        self.assertEqual(a, 5)
        self.assertEqual(b, 3)

        # shared reference since they are mutable
        coll1 = [2,3,4]
        coll2 = coll1
        self.assertEqual(coll1, [2,3,4])
        self.assertEqual(coll2, [2,3,4])

        # coll1 is immutable and no shared reference
        coll1 = 24
        self.assertEqual(coll1, 24)
        self.assertEqual(coll2, [2,3,4])

    def test_copy(self):
        coll1 = [2,3,4]
        coll2 = coll1[:]
        coll3 = list(coll1)
        coll1[0] = 24

        self.assertEqual(coll1, [24,3,4])
        self.assertEqual(coll2, [2,3,4])
        self.assertEqual(coll3, [2,3,4])


    # name, reference, do not have type
    def test_reference_no_type(self):
        a = 5
        self.assertEqual(type(a), type(int()))

        a = 'foo'
        self.assertEqual(type(a), type(str()))

        a = [1,2,3]
        self.assertEqual(type(a), type(list()))


#={===========================================================================
# py-numeric

class TestNumeric(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_integer_division(self):

        # fpr *py-2*
        # self.assertEqual(3 / 2, 1)
        # for *py-3*
        self.assertEqual(3 / 2, 1.5)
        self.assertEqual(3 / float(2), 1.5)

        # self.assertEqual(21600/90000, 0)
        self.assertEqual(21600/90000, 0.24)
        self.assertEqual(21600/float(90000), 0.24)

        # SyntaxError: from __future__ imports must occur at the beginning of the file
        # from __future__ import division
        # self.assertEqual(3 / 2, 1.5)

    def test_chained_comparison(self):
        a = 10
        b = 11
        c = 12

        self.assertEqual(a < b < c, True)

    # [RUN]  test_random
    # 0.146921784216
    # 5
    # 5
    # Meaning of Life
    # Meaning of Life
    # ['clubs', 'diamonds', 'spades', 'hearts']
    # ['diamonds', 'clubs', 'hearts', 'spades']

    def test_random(self):
        import random
        print(random.random())

        print(random.randint(1, 10))
        print(random.randint(1, 10))

        coll1 = ['Life of Brian', 'Holy Grail', 'Meaning of Life']
        print(random.choice(coll1))
        print(random.choice(coll1))

        coll2 = ['hearts', 'clubs', 'diamonds', 'spades']
        random.shuffle(coll2)
        print(coll2)
        random.shuffle(coll2)
        print(coll2)

    def test_min(self):
        coll = ['hearts', 'clubs', 'diamonds', 'spades']
        result = min(coll)
        self.assertEqual(result, 'clubs')


#={===========================================================================
# py-range

class TestRange(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    # With one argument, range generates a list of integers from zero up to but
    # not including the argument's value. 
    
    # If you pass in two arguments, the first is taken as the lower bound. An
    # optional third argument can give a step; if it is used, Python adds the
    # step to each successive integer in the result (the step defaults to +1).

    # Ranges can also be nonpositive and nonascending, if you want them to be:

    def test_range(self):

        self.assertEqual(list(range(5)), [0,1,2,3,4])

        self.assertEqual(list(range(3, 5)), [3,4])

        # py-range py-3
        coll = list(range(10))
        self.assertEqual(coll, [0, 1, 2, 3, 4, 5, 6, 7, 8, 9])

        #
        coll2 = [0] * 4;
        self.assertEqual(coll2, [0, 0, 0, 0])

        coll3 = [5,6,7,8]

        for i in range(len(coll2)):
            coll2[i] = coll3[i]

        self.assertEqual(coll2, coll3)

        # xrange() is for py-2
        # # py-modulo
        # sum = 0
        # for i in list(xrange(10000)):

        #     # py-way no precedence between ops?
        #     if i % 3 == 0 or i % 5 == 0:
        #         sum += i

        # self.assertEqual(sum, 23331668)


#={===========================================================================
# py-string

class TestString(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_string_conversion(self):
        self.assertEqual(int('42'), 42)
        self.assertEqual(int('1101'), 1101)
        self.assertEqual(str(42), '42')
        self.assertEqual(repr(42), '42')

        # binary to int
        self.assertEqual(int(0b1101), 13)

        # string to binary
        self.assertEqual(int('1101', 2), 13)

        # int to binary
        self.assertEqual(bin(13), '0b1101')


    # $ python3 -m unittest pycore.TestString.test_string_split
    def test_string_split(self):

        self.assertEqual(' 1 2  3   '.split(), ['1', '2', '3'])
        self.assertEqual(' 1 2  3   '.split(None, 1), ['1', '2  3   '])

        # Delimiters can be longer than a single character, too:

        line = "i'mSPAMaSPAMlumberjack"
        self.assertEqual(line.split("SPAM"), ["i'm", 'a', 'lumberjack'])

        # see how delimeter works
        i = str(11)
        self.assertEqual(i.split(str(1)), ['','',''])

        i = str(12)
        self.assertEqual(i.split(str(1)), ['','2'])

        coll = 'hello python'
        self.assertEqual(coll.split(), ['hello', 'python'])

        # ======================================================================
        # ERROR: test_string_split (__main__.TestString)
        # ----------------------------------------------------------------------
        # Traceback (most recent call last):
        #   File "pycore.py", line 191, in test_string_split
        #     coll.split(2)
        # TypeError: expected a character buffer object

        with self.assertRaises(TypeError):
            coll.split(2)

    # str.splitlines([keepends])
    # Return a list of the lines in the string, breaking at line boundaries.
    # This method uses the universal newlines approach to splitting lines. Line
    # breaks are not included in the resulting list unless keepends is given and
    # true.

    def test_string_splitlines(self):

        lines = 'Milk\nChicken\r\nBread\rButter'

        self.assertEqual(lines.splitlines(), 
                ['Milk', 'Chicken', 'Bread', 'Butter'])

        self.assertEqual(lines.splitlines(True), 
                ['Milk\n', 'Chicken\r\n', 'Bread\r', 'Butter'])

    def test_string_rstrip(self):
        self.assertEqual('   spacious   '.rstrip(), '   spacious')

        coll = '   spacious   '
        self.assertEqual(coll[:-1], '   spacious  ')

        self.assertEqual('mississippi'.rstrip('ipz'), 'mississ')

    def test_string_join(self):
        coll = ['pay', 'job', 'name']
        self.assertEqual(','.join(coll), 'pay,job,name')
        self.assertEqual('_'.join(coll), 'pay_job_name')
        self.assertEqual('*'.join(coll), 'pay*job*name')
        self.assertEqual('_'.join('pay'), 'p_a_y')

    def test_string_replace(self):

        coll = 'xxxxSPAMxxxxSPAMxxxx'

        # replace all
        self.assertEqual(coll.replace('SPAM', 'EGGS'),
                'xxxxEGGSxxxxEGGSxxxx')

        # replace one
        self.assertEqual(coll.replace('SPAM', 'EGGS', 1),
                'xxxxEGGSxxxxSPAMxxxx')

    def test_string_title(self):

        coll = 'this is python title method'
        self.assertEqual(coll.title(),
                'This Is Python Title Method')

    def test_string_performance(self):

        # to list
        coll = 'spammy'
        l = list(coll)

        l[4] = l[3] = 'x'

        # to string it back
        coll = ''.join(l)
        self.assertEqual(coll, 'spaxxy')

    def test_string_concat(self):

        coll = "test" + "_" + "string" + "_" + "concat";
        self.assertEqual(coll, "test_string_concat")


#={===========================================================================
# py-tuple

class TestTuple(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_tuple_one_item(self):

        # one item tuple
        coll = (40,)

        # group expression, integer
        x = (40)

        self.assertEqual(type(coll), type((40,50)))
        self.assertEqual(x, 40)

    def test_tuple_ctor(self):

        # use comma-separated sequence to create
        coll1 = 4,5,6
        coll2 = (4,5,6)
        self.assertEqual(coll1, coll2)

        # nested tuple
        coll3 = (4,5,6),(7,8)
        self.assertEqual(coll3, ((4, 5, 6), (7, 8)))

        # py-convert
        # any sequence or iterator can be converted
        coll4 = tuple([4,5,6])
        self.assertEqual(coll4, coll1)

        coll5 = tuple('string')
        self.assertEqual(coll5, ('s', 't', 'r', 'i', 'n', 'g'))

        # py-access
        self.assertEqual(coll5[0], 's')
        self.assertEqual(coll5[1], 't')
        self.assertEqual(coll5[2], 'r')

        # operator+()
        coll6 = coll2 + coll3 + coll5
        self.assertEqual(coll6, 
                (4, 5, 6, (4, 5, 6), (7, 8), 's', 't', 'r', 'i', 'n', 'g'))


    # ======================================================================
    # ERROR: test_tuple_immutable (__main__.TestTuple)
    # ----------------------------------------------------------------------
    # Traceback (most recent call last):
    #   File "pycore.py", line 278, in test_tuple_immutable
    #     coll1[1] = 7
    # TypeError: 'tuple' object does not support item assignment

    def test_tuple_immutable(self):

        coll1 = (4,5,6)

        with self.assertRaises(TypeError):
            coll1[1] = 7


    def test_tuple_op(self):

        coll1 = 1,2,2,3,4,2
        self.assertEqual(coll1.count(2), 3) 

        # py-membership
        coll2 = ('foo', 'bar', 'baz')
        result = 'bar' in coll2
        self.assertEqual(result, True)


#={===========================================================================
# py-list

class TestList(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)


    # convert to list
    def test_list_ctor(self):

        coll1 = (1,2,3,4)
        coll2 = list(coll1)

        self.assertEqual(type(coll1), type((1,)))
        self.assertEqual(type(coll2), type([1]))

    def test_list_ctor_empty(self):
        coll1 = list()
        self.assertEqual(type(coll1), type([1]))
        self.assertEqual(len(coll1), 0)

        coll2 = []
        self.assertEqual(type(coll2), type([1]))
        self.assertEqual(len(coll2), 0)

    def test_list_delete(self):

        coll = ['spam', 'eggs', 'ham', 'toast']

        # delete one item
        del coll[0]
        self.assertEqual(coll, ['eggs', 'ham', 'toast'])

        # delete an entire section
        del coll[1:]
        self.assertEqual(coll, ['eggs'])

        # slice can do the same
        coll2 = ['spam', 'eggs', 'ham', 'toast']

        coll2[:1] = []
        self.assertEqual(coll2, ['eggs', 'ham', 'toast'])

        coll2[1:] = []
        self.assertEqual(coll2, ['eggs'])

    def test_list_append_tuple(self):
        coll1 = []
        coll1.append('spam')
        coll1.append(tuple([1,2]))
        coll1.append('eggs')
        # print(coll)
        self.assertEqual(coll1, ['spam', (1, 2), 'eggs'])

        coll2 = []
        coll2.append('spam')
        x = (1,2)
        coll2.append(x)
        coll2.append('eggs')
        # print(coll)
        self.assertEqual(coll2, ['spam', (1, 2), 'eggs'])

    def test_list_ops(self):

        coll = ['spam', 'eggs', 'ham', 'toast']

        coll.append('foo')
        self.assertEqual(coll, 
                ['spam', 'eggs', 'ham', 'toast', 'foo'])

        coll.insert(1, 'bean')
        self.assertEqual(coll, 
                ['spam', 'bean', 'eggs', 'ham', 'toast', 'foo'])

        # py-list-pop
        # `pop` removes and returns an element at a particular index:
        # Remove the item at the given position in the list, and return it. If
        # no index is specified, a.pop() removes and returns the last item in
        # the list.

        coll = ['spam', 'eggs', 'ham', 'toast']
        coll.pop(2)
        self.assertEqual(coll, 
                ['spam', 'eggs', 'toast'])

        coll.pop()
        self.assertEqual(coll, 
                ['spam', 'eggs'])

        # py-list-remove
        # search the first and remove it

        coll = ['foo', 'red', 'red', 'red', 'baz', 'dwarf']
        coll.remove('red')
        self.assertEqual(coll, 
                ['foo', 'red', 'red', 'baz', 'dwarf'])

        # py-list-extend
        coll = ['foo', 'red']
        coll.extend(['red', 'baz', 'dwarf'])
        self.assertEqual(coll, 
                ['foo', 'red', 'red', 'baz', 'dwarf'])

    def test_list_concatenate(self):

        coll1 = [4, None, 'foo'] + [7,8,(2,3)]
        self.assertEqual(coll1, 
            [4, None, 'foo', 7, 8, (2, 3)])

        # py-list-extend
        coll2 = [4, None, 'foo']
        coll2.extend([7,8,(2,3)])
        self.assertEqual(coll2, 
            [4, None, 'foo', 7, 8, (2, 3)])


    def test_list_sort(self):

        # case matters
        L = ['abc', 'ABD', 'aBe']
        L.sort()
        self.assertEqual(L, ['ABD', 'aBe', 'abc'])

        # normalize to lowercase
        L = ['abc', 'ABD', 'aBe']
        L.sort(key=str.lower)
        self.assertEqual(L, ['abc', 'ABD', 'aBe'])

        # py-reverse
        # both sort() and reverse() methods modify the list directly.
        # normalize to lowercase and reverse
        L = ['abc', 'ABD', 'aBe']
        L.sort(key=str.lower, reverse=True)
        self.assertEqual(L, ['aBe', 'ABD', 'abc'])

        L = ['saw', 'small', 'He', 'foxes', 'six']
        L.sort(key=len)
        self.assertEqual(L, ['He', 'saw', 'six', 'small', 'foxes'])

        # LPY5, Chapter 4: Introducing Python Object Types
        M = ['bb', 'aa', 'cc']
        M.reverse() 
        self.assertEqual(M, ['cc', 'aa', 'bb'])

        # py-sorted or py-list-sort?
        coll1 = ["flower","flow","flight"]
        coll1.sort()

        self.assertEqual(coll1, ['flight', 'flow', 'flower'])

        coll2 = sorted(coll1)

        self.assertEqual(coll1, coll2)


    # [RUN]  test_list_elements
    # ['l', 'i', 's', 't', 1, 2, 3, 'type']

    def test_list_elements(self):
        coll = list('list')
        coll.append(1)
        coll.append(2)
        coll.append(3)
        coll.append('type')
        print(coll)

    def test_list_nested(self):
        coll = [
                [["AMS"], ["AMS"]], 
                [["PROX"], ["darwin"]],
                # can have comment in the middle.  
                [["PPCM_CF"], ["ppcm", "ppcm_core"]] 
               ]

        self.assertEqual(coll[0], [["AMS"], ["AMS"]])
        self.assertEqual(coll[0][0], ["AMS"])

        # Q: why cannot use multiple lines?

        coll = "[[['AMS'], ['AMS']], [['PROX'], ['darwin']], [['PPCM_CF'], ['ppcm', 'ppcm_core']]]"
        self.assertEqual(type(coll), type(str()))

        coll = eval("[[['AMS'], ['AMS']], [['PROX'], ['darwin']], [['PPCM_CF'], ['ppcm', 'ppcm_core']]]")
        self.assertEqual(type(coll), type(list()))

    # difference between "a[:] = b" and "a = b"
    # py-reference py-copy
    def test_list_copy_and_reference(self):
        coll1 = [1,2,3]
        coll2 = [4,5,6]

        # copy, smae contents but different colls
        coll1[:] = coll2
        self.assertNotEqual(id(coll1), id(coll2))

        # reference, smae colls
        coll1 = coll2
        self.assertEqual(id(coll1), id(coll2))


#={===========================================================================
# py-set

class TestSet(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_set_ctor(self):

        # use built-in function
        coll1 = set([2,2,2,1,3,3])
        self.assertEqual(coll1, {1,2,3})

        # a new set literal form, using the curly braces formerly reserved for
        # dictionaries. In 3.X and 2.7, the following are equivalent

        coll2 = {2,2,2,1,3,3}
        self.assertEqual(coll2, {1,2,3})

    def test_set_ops(self):

        a = {1,2,3,4,5}
        b = {3,4,5,6,7,8}

        # union(or)
        self.assertEqual(a | b, {1, 2, 3, 4, 5, 6, 7, 8})

        # intersection(and)
        self.assertEqual(a & b, {3, 4, 5})

        # difference
        self.assertEqual(a - b, {1, 2})
        # differences, filter duplicates
        self.assertEqual({1,3,5,7} - {1,2,4,5,6}, {3,7})

        # symmetric difference
        self.assertEqual(a ^ b, {1, 2, 6, 7, 8})

        # super or sub
        self.assertTrue({1,2,3}.issubset(a))
        self.assertTrue(a.issuperset({1,2,3}))

        # equal
        # the order matters in py-list but not in py-set. Can user sorted() on
        # list to have the same result.

        self.assertTrue({1,2,3} == {3,2,1})
        self.assertFalse([1,2,3] == [3,2,1])

        # sets can only contain immutable (a.k.a. "hashable") object types as
        # keys in dict does. so tuple is okay and note that set ctor can use
        # listlist.
        #
        # TypeError: unhashable type: 'list'

        coll = set()
        coll.add(1.23)
        coll.add((1,2,3))
        self.assertEqual(coll, {1.23, (1, 2, 3)})

        z = {'b', 'd'}
        z.add('SPAM')
        self.assertEqual(z, {'b', 'd', 'SPAM'})

        # merge: in-place union
        z.update(set(['X', 'Y']))
        self.assertEqual(z, {'Y', 'X', 'b', 'd', 'SPAM'})

        z.remove('b') # Delete one item
        self.assertEqual(z, {'Y', 'X', 'd', 'SPAM'})

    def test_set_iteration(self):
        
        for item in set('abc'):
            print(item * 3)

        coll = {x ** 2 for x in [1,2,3,4]}
        self.assertEqual(coll, {1,4,9,16})


#={===========================================================================
# py-dict

class TestDict(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_dict_key_type(self):

        hash('string')
        # -1542666171

        hash((1,2,(2,3)))
        # 1387206534

        # TypeError: unhashable type: 'list'
        with self.assertRaises(TypeError):
            hash((1,2,[2,3]))
            

    def test_dict_ctor(self):

        # create dict by assign
        # is handy if you can spell out the entire dictionary ahead of time.

        d = {}
        d['name'] = 'Bob'
        d['age'] = 40
        self.assertEqual( d, {'name': 'Bob', 'age': 40})

        # create dict by initializers
        # The second is of use if you need to create the dictionary one field at
        # a time on the fly.

        d = {'name': 'Bob', 'age': 40}
        self.assertEqual( d, {'name': 'Bob', 'age': 40})


        # create dict by keyword argument form
        # The third involves less typing than the first, but it requires all
        # keys to be strings.

        d = dict(name='Bob', age=40)
        self.assertEqual( d, {'name': 'Bob', 'age': 40})

        # create dict by key, value tuple form
        # The last is useful if you need to build up keys and values as
        # sequences at runtime.

        d = dict([('name', 'Bob'), ('age', 40)])
        self.assertEqual( d, {'name': 'Bob', 'age': 40})


    # ('a', '=>', 100)
    # ('c', '=>', 300)
    # ('b', '=>', 200)
    # --------------
    # ('a', '=>', 100)
    # ('b', '=>', 200)
    # ('c', '=>', 300)

    def test_dict_sort(self):

        # in recent versions of Python it can be done in one step with the newer
        # `sorted()` built-in function.

        d = {'a':100, 'c':300, 'b':200}

        # same as 'for key in d.keys():'
        for key in d:
            print(key, '=>', d[key])

        print('--------------')

        for key in sorted(d):
            print(key, '=>', d[key])

        # use list-sort member 

        d = {'a':100, 'c':300, 'b':200}

        for key in d:
            print(key, '=>', d[key])

        key_list = list(d.keys())
        key_list.sort()

        print('--------------')

        for key in key_list:
            print(key, '=>', d[key])


    def test_dict_iterator(self):

        d = {'a':100, 'c':300, 'b':200}

        if 'a' in d:
            print('exist')
        else: 
            print('not exist')


    def test_dict_insert(self):

        # *py-dict-update* insert

        # note that b is overwritten
        # {'a': 'some value', 'b': [1, 2, 3, 4], 7: 'an integer'}
        # {'a': 'some value', 'c': 12, 'b': 'foo', 7: 'an integer'}

        d = {'a': 'some value', 'b': [1, 2, 3, 4], 7: 'an integer'}
        self.assertEqual(d, 
                {'a': 'some value', 'b': [1, 2, 3, 4], 7: 'an integer'})

        d.update({'b':'foo','c':12})
        self.assertEqual(d, 
                {'a': 'some value', 'c': 12, 'b': 'foo', 7: 'an integer'})

        # update and insert a item.
        d = {'a': 'some value', 'b': [1, 2, 3, 4], 7: 'an integer'}
        d['b'] = 'foo'
        d['c'] = 12
        self.assertEqual(d, 
                {'a': 'some value', 'c': 12, 'b': 'foo', 7: 'an integer'})


    def test_dict_ops(self):

        d = {'a': 'some value', 'dummy': 'another value', 'b': [1, 2, 3, 4], 5: 'some value', 7: 'an integer'}
        self.assertEqual(d, 
                {'a': 'some value', 'dummy': 'another value', 'b': [1, 2, 3, 4], 5: 'some value', 7: 'an integer'})

        # *py-del*
        del d[5]
        self.assertEqual(d, 
                {'a': 'some value', 'dummy': 'another value', 'b': [1, 2, 3, 4], 7: 'an integer'})

        d.pop('dummy')
        self.assertEqual(d, 
                {'a': 'some value', 'b': [1, 2, 3, 4], 7: 'an integer'})


        # *py-dict-keys* 
        # use print() since the order of result varies.
        #
        # ['a', 'dummy', 'b', 5, 7]
        # ['some value', 'another value', [1, 2, 3, 4], 'some value', 'an integer']

        d = {'a': 'some value', 'dummy': 'another value', 'b': [1, 2, 3, 4], 5: 'some value', 7: 'an integer'}

        print(d.keys())
        print(d.values())


        # items()
        # Return a copy of the dictionary's list of (key, value) pairs.
        # searching through sequences like this is generally much slower 
        # than a direct key index

        # [('The Meaning of Life', '1983'), ('Life of Brian', '1979'), ('Holy Grail', '1975')]
        # ['Holy Grail']

        # py-dict-items, Key, Value
        d = {'Holy Grail': '1975', 'Life of Brian': '1979','The Meaning of Life': '1983'}

        # *py-comprehension*
        self.assertEqual(
                [title for title, year in d.items() if year == '1975'], 
                ['Holy Grail'])


        # get(key[, default])
        # Return the value for key if key is in the dictionary, else default. If
        # default is not given, it defaults to None, so that this method never
        # raises a KeyError.

        d = {'a':100, 'c':300, 'b':200}

        self.assertEqual(d.get('a', 400), 100)
        self.assertEqual(d.get('a'), 100)
        self.assertEqual(d.get('d', 400), 400)


        # Suppose that there are log lines which logs creation and deletion of a handle.
        # So use a handle as a key and see occurances. If occurances is 1 then deletion
        # of a handle is missing so leaks resources. The code below is for this case. 
        # 
        # If want to map a line as well, for example, 

        #  {handle, [occurance, line]}
        #
        # how can we support this? The below give an error:

        d['0x123'] = d.get('0x123', [0, ''])[0]+1

        # more compact than if/else

        # if 'a' not in d:
        #     d['a'] = 1
        # else:
        #     d['a'] += 1

        # if 'a' not in d:
        #     d['a'] = 1
        # else:
        #     d['a'] += 1

        # if 'b' not in d:
        #     d['b'] = 1
        # else:
        #     d['b'] += 1

        d = {}

        d['0x123'] = d.get('0x123', 0)+1
        d['0x123'] = d.get('0x123', 0)+1
        d['0x345'] = d.get('0x345', 0)+1
        d['0x678'] = d.get('0x678', 0)+1
        d['0x678'] = d.get('0x678', 0)+1
        d['0x145'] = d.get('0x145', 0)+1

        # {'0x145': 1, '0x678': 2, '0x123': 2, '0x345': 1}

        result = [handle for (handle, occurance) in d.items() if occurance == 1]
        command = 'egrep -an "' + '|'.join(result) + '"'

        # cannot use this since the result list varies in order
        # self.assertEqual(command, 'egrep -an "0x145|0x345"')

        # for (handle, occurance) in d.items():
        #     if occurance == 1:
        #         print handle


        # group input words by the first char of a word
        # {'a': ['apple', 'atom'], 'b': ['bat', 'bar', 'book']}

        words=['apple', 'bat', 'bar', 'atom', 'book']
 
        by_letter={}
 
        for word in words:
            letter = word[0]
            if letter not in by_letter:
                by_letter[letter] = [word]
            else:
                by_letter[letter].append(word)
 
        print(by_letter)


        # the same result

        # setdefault(key[, default])
        # If key is in the dictionary, return its value. If not, insert key with
        # a value of default and return default. default defaults to None.

        words=['apple', 'bat', 'bar', 'atom', 'book']
 
        by_letter={}
 
        for word in words:
            letter = word[0]
            by_letter.setdefault(letter, []).append(word)
 
        print(by_letter)


#={===========================================================================
# py-coll-deque

from collections import deque

class TestCollDeque(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    # When new items are added and the queue is full, the oldest item is
    # automatically removed.

    def test_coll_deque(self):
        q = deque(maxlen = 3)
        q.append(1)
        q.append(2)
        q.append(3)

        # AssertionError: deque([1, 2, 3], maxlen=3) != [1, 2, 3]
        # self.assertEqual(q, [1,2,3])
        self.assertEqual(list(q), [1,2,3])

        q.append(4)
        self.assertEqual(list(q), [2,3,4])

        q.append(5)
        self.assertEqual(list(q), [3,4,5])

        q = deque()
        q.append(1)
        q.append(2)
        q.append(3)
        self.assertEqual(list(q), [1,2,3])

        q.appendleft(4)
        self.assertEqual(list(q), [4,1,2,3])

        q.pop()
        self.assertEqual(list(q), [4,1,2])

        q.popleft()
        self.assertEqual(list(q), [1,2])

    # *py-generator*
    # search() yields the matched line and five previous lines from that

    @staticmethod
    def search(lines, pattern, history = 5):
        previous_lines = deque(maxlen = history)

        for line in lines:

            if pattern in line:
                yield line, previous_lines

            previous_lines.append(line)

    def test_cookbook_1_3(self):
        with open('some.txt') as f:
            for line, previous_lines in TestCollDeque.search(f, 'python', 5):
                for num, pline in enumerate(previous_lines):
                    print(num, ':', pline, end='')
                print(line, end='')
                print('-'*20)


#={===========================================================================
# py-coll-heapq

import heapq

class TestCollHeapq(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_coll_heapq(self):
        coll = [1, 8, 2, 23, 7, -4, 18, 23, 42, 37, 2]

        self.assertEqual(heapq.nlargest(3, coll), 
                [42,37,23])

        self.assertEqual(heapq.nsmallest(3, coll), 
                [-4,1,2])

        # same result
        coll = sorted([1, 8, 2, 23, 7, -4, 18, 23, 42, 37, 2])
        coll_reversed = sorted([1, 8, 2, 23, 7, -4, 18, 23, 42, 37, 2],
                reverse=True)

        # nlargest, different order
        self.assertEqual(coll[-3:], 
                [23,37,42])

        # nlargest
        self.assertEqual(coll_reversed[:3], 
                [42,37,23])

        # nsmallest
        self.assertEqual(coll[:3], 
                 [-4,1,2])

    # def test_cookbook_1_4(self):


#={===========================================================================
# py-slice

class TestSlice(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_slice_and_index(self):
        coll = [7,2,3,7,5,6,0,1]

        # index is different from slice
        self.assertEqual(coll[2], 3)

        # from right, [..., -2, -1, end())
        self.assertEqual(coll[-2], 0)

        # makes a single item list
        self.assertEqual(coll[1:2], [2])

        self.assertEqual(coll[1:5], [2, 3, 7, 5])

        # py-list-insert since assign
        coll[3:4] = [6,3]
        self.assertEqual(coll, [7, 2, 3, 6, 3, 5, 6, 0, 1])

        # py-list-delete
        coll[3:4] = []
        self.assertEqual(coll, [7, 2, 3, 3, 5, 6, 0, 1])

    def test_slice_default(self):

        coll = [7,2,3,7,5,6,0,1]

        self.assertEqual(coll[1:], [2, 3, 7, 5, 6, 0, 1])

        # coll do not change
        self.assertEqual(coll, [7, 2, 3, 7, 5, 6, 0, 1])

        self.assertEqual(coll[:3], [7, 2, 3])

        self.assertEqual(coll[:-1], [7, 2, 3, 7, 5, 6, 0])

        # all, py-copy
        self.assertEqual(coll[:], coll)

        # to get the last four chars
        message = '0123456789'
        self.assertEqual(message[-4:], '6789')

    def test_slice_step(self):

        coll = [7,2,3,6,3,5,6,0,1]

        self.assertEqual(coll[::2], [7,3,3,6,1])

        # py-list-reverse py-reverse
        # A clever use of this is to pass -1 which has the useful effect of
        # reversing a list or tuple:

        self.assertEqual(coll[::-1], [1, 0, 6, 5, 3, 6, 3, 2, 7])


#={===========================================================================
# py-none

class TestNone(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_none_empty(self):

        coll1 = ''

        if coll1:
            result = True
        else:
            result = False

        self.assertEqual(result, False)

        coll2 = None

        if coll2:
            result = True
        else:
            result = False

        self.assertEqual(result, False)

        coll1 = dict()

        if coll1:
            result = True
        else:
            result = False

        self.assertEqual(result, False)

        coll1['a'] = (1, 2)

        if coll1:
            result = True
        else:
            result = False

        self.assertEqual(result, True)

        # empty list
        coll1 = []

        if not coll1:
            result = False

        self.assertEqual(result, False)


#={===========================================================================
# py-print

class TestPrint(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    # [RUN]  test_print_function
    # item in the list: [1, 2, 3, 4]
    # [1, 2, 3, 4]
    # [1, 2, 3, 4]
    # 10000.00
    # 10000.00
    # 10000.00
    # spam, eggs, and SPAM!
    # spam, eggs, and SPAM!
    # spam, eggs, and SPAM!
    # 100, 40, 1000000
    # 42 -- 3.14159 -- [1, 2, 3]

    def test_print_function(self):
        item = [1,2,3,4]

        print('item in the list: %s' % item)

        # *py-2x*
        # print item

        print(item)

        pay = 10000
        print('%.2f' % pay)
        print('{0:.2f}'.format(pay))

        # numbers are optional
        print('{:.2f}'.format(pay))

        print('%s, eggs, and %s' % ('spam', 'SPAM!')) 

        print('{0}, eggs, and {1}'.format('spam', 'SPAM!'))

        # Numbers optional (2.7+, 3.1+)
        print('{}, eggs, and {}'.format('spam', 'SPAM!'))

        print('{0:o}, {1:x}, {2:b}'.format(64, 64, 64))

        print('%s -- %s -- %s' % (42, 3.14159, [1, 2, 3]))


    # [RUN]  test_print_multiple_lines
    # this is lien one
    #             this is line two
    #             this is line three
    # this is lien one
    # this is line two
    # this is line three

    def test_print_multiple_lines(self):

        print("""this is lien one
            this is line two
            this is line three""")

        print("""this is lien one
            \rthis is line two
            \rthis is line three""")

    # "one two three" for py2
    # def test_print_multiple(self):
    #     print 'one', 
    #     print 'two', 
    #     print 'three'

    # three lines
    def test_print_multiple_args(self):
        print('one'), 
        print('two'), 
        print('three')


#={===========================================================================
# py-base

class TestBase(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_base_pass(self):
        value = 10

        if value < 0:
            result = 'negative'
        elif value == 0:
            # TODO: put something smart here
            pass
        else:
            result = 'positive'

        self.assertEqual(result, 'positive')

    # py-error compile error, when miss out `pass`
    # IndentationError: expected an indented block
    #
    # def test_pass_error(self):
    #     value = 10
    #
    #     if value < 0:
    #         result = 'negative'
    #     elif value == 0:
    #         # TODO: put something smart here
    #     else:
    #         result = 'positive'
    #
    #     self.assertEqual(result, 'positive')

    def isiterable(self, object):
        try:
            iter(object)
            return True
        except TypeError:
            return False

    def test_base_iterable(self):
        self.assertTrue(self.isiterable('string'))
        self.assertTrue(self.isiterable([1,2,3]))
        self.assertFalse(self.isiterable(4))


    def compare_version_1(self, lhs, rhs):
        # makes int list
        lhs = [int(i) for i in lhs.split('.')]
        rhs = [int(i) for i in rhs.split('.')]

        if lhs > rhs:
            return 1
        elif lhs < rhs:
            return -1
        else:
            return 0

    # use string comparison itself
    def compare_version_2(self, lhs, rhs):

        if lhs > rhs:
            return 1
        elif lhs < rhs:
            return -1
        else:
            return 0

    def test_base_compare(self):

        # py-comparison
        coll1 = [1, ('a', 3)]
        coll2 = [1, ('a', 2)]
        # Less, equal, greater: tuple of results
        result = coll1 < coll2, coll1 == coll2, coll1 > coll2
        self.assertEqual(result, (False, False, True))

        version1 = '40.65.00'
        version2 = '40.66.00'
        version3 = '40.66.00'

        self.assertEqual(self.compare_version_1(version1, version2), -1)
        self.assertEqual(self.compare_version_1(version2, version1), 1)
        self.assertEqual(self.compare_version_1(version2, version3), 0)

        self.assertEqual(self.compare_version_2(version1, version2), -1)
        self.assertEqual(self.compare_version_2(version2, version1), 1)
        self.assertEqual(self.compare_version_2(version2, version3), 0)


    def test_base_ternary(self):

        coll = 'spam'

        a = 'true' if coll else 'false'

        self.assertEqual(a, 'true')

    # py-eval
    def test_base_eval(self):
        # Q: why cannot use multiple lines?

        coll = "[[['AMS'], ['AMS']], [['PROX'], ['darwin']], [['PPCM_CF'], ['ppcm', 'ppcm_core']]]"
        self.assertEqual(type(coll), type(str()))

        coll = eval("[[['AMS'], ['AMS']], [['PROX'], ['darwin']], [['PPCM_CF'], ['ppcm', 'ppcm_core']]]")
        self.assertEqual(type(coll), type(list()))


#={===========================================================================
# py-reference

class TestReference(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_reference(self):

        a = [1,2,3]
        b = a

        c = list(a)
        # *py-copy*
        cc = a[:]

        d = [1,2,3]

        # since py-reference py-is
        self.assertTrue(a is b)
        self.assertTrue(a == b)

        # c is a different list
        self.assertTrue(a is not c)

        # c and cc is a different list
        self.assertTrue(c is not cc)
        self.assertTrue(c == cc)

        # same value
        self.assertTrue(a == d)

        # but different object
        self.assertFalse(a is d)


#={===========================================================================
# py-sorted py-sort

class TestSorted(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_sorted(self):

        # sorted do not change coll1

        coll1 = ['abc', 'ABD', 'aBe']
        result = sorted(coll1)
        self.assertEqual(result, ['ABD', 'aBe', 'abc'])

        result = sorted(coll1, key=str.lower, reverse=True)
        self.assertEqual(result, ['aBe', 'ABD', 'abc'])

        # input is not coll2 since it's lower cased before running sorted()

        coll2 = ['abc', 'ABD', 'aBe']
        result = sorted([x.lower() for x in coll2], reverse=True)
        self.assertEqual(result, ['abe', 'abd', 'abc'])

        coll3 = sorted([7,1,2,6,0,3,2,3,2])
        self.assertEqual(coll3, [0, 1, 2, 2, 2, 3, 3, 6, 7])

        coll3 = sorted(set([7,1,2,6,0,3,2,3,2]))
        self.assertEqual(coll3, [0, 1, 2, 3, 6, 7])


#={===========================================================================
# py-to tidy up

class TestXXXX(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    # `reversed` iterates over the elements of a sequence in reverse order and
    # return iterator object so have to make it list from it
    def test_reverse(self):
        L = list(reversed(range(10)))
        self.assertEqual(L, [9, 8, 7, 6, 5, 4, 3, 2, 1, 0])


#={===========================================================================
# py-bisect py-search

class TestBisect(unittest.TestCase):
    
    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_bisect(self):

        import bisect

        coll = [1,2,2,2,3,4,7]
        result = bisect.bisect(coll, 2)
        self.assertEqual(result, 4)

        coll = [1,2,2,2,3,4,7]
        bisect.insort(coll, 6)
        self.assertEqual(coll, 
                [1, 2, 2, 2, 3, 4, 6, 7])


#={===========================================================================
# py-unpack

class TestUnpack(unittest.TestCase):
    
    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_unpack(self):

        coll1 = (4,5,6)
        a, b, c = coll1

        self.assertEqual(a, 4)
        self.assertEqual(b, 5)
        self.assertEqual(c, 6)

        # py-swap, uses temporary tuple?

        # Using this functionality it's easy to swap `variable names`, a task
        # which in many languages might look like:
        # 
        # tmp = a 
        # a = b 
        # b = tmp

        a, b = b, a

        self.assertEqual(a, 5)
        self.assertEqual(b, 4)

        # py-iterator

        coll2 = (4,5,6)
        result = []
        for a in coll2:
            result.append(a)
        self.assertEqual(result, [4,5,6])

        coll3 = [(1, 2, 3), (4, 5, 6), (7, 8, 9)]
        result = []
        for a, b, c in coll3:
            result.append(a)
        self.assertEqual(result, [1,4,7])

        # PYCB3, Chapter 1: Data Structures and Algorithms
        # 1.1. Unpacking a Sequence into Separate Variables
        #
        # The only requirement is that the number of variables and structure
        # match the sequence.

        # py-error
        #    for a, b, c, d in coll3:
        # ValueError: need more than 3 values to unpack

        with self.assertRaises(ValueError):
            coll3 = [(1, 2, 3), (4, 5, 6), (7, 8, 9)]
            for a, b, c, d in coll3:
                print(a)


        # When unpacking, you may sometimes want to discard certain values.
        # Python has no special syntax for this, but you can often just pick a
        # throwaway variable name for it.               

        data = [ 'ACME', 50, 91.1, (2012, 12, 21) ]
        _, share, price, _ = data

        self.assertEqual(share, 50)
        self.assertEqual(price, 91.1)


    # PYCB3, Chapter 1: Data Structures and Algorithms
    # 1.2. Unpacking Elements from Iterables of Arbitrary Length
    #
    # Problem
    # You need to unpack N elements from an iterable, but the iterable may be
    # longer than N elements, causing a "too many values to unpack" exception.
    #
    # *py-3*
    # starred variable will always be a list, *py-list*

    @staticmethod
    def do_foo(x, y):
        print('foo', x, y)

    @staticmethod
    def do_bar(s):
        print('bar', s)

    def test_unpack_star_expression(self):

        record = ('Dave', 'dave@example.com', '773-555-1212', '847-555-1212')
        name, email, *phone_numbers = record

        self.assertEqual(name, 'Dave')
        self.assertEqual(email, 'dave@example.com')
        self.assertEqual(phone_numbers, ['773-555-1212', '847-555-1212'])

        *trailing, current = [10, 8, 7, 1, 9, 5, 10, 3]
        self.assertEqual(trailing, [10, 8, 7, 1, 9, 5, 10])


        # can be especially useful when iterating over a sequence of tuples of
        # varying length.

        records = [ 
                ('foo', 1, 2), 
                ('bar', 'hello'), 
                ('foo', 3, 4) 
                ]

        for tag, *args in records:
            if tag == 'foo':
                TestUnpack.do_foo(*args)
            elif tag == 'bar':
                TestUnpack.do_bar(*args)


        # py-string processing

        line = 'nobody:*:-2:-2:Unprivileged User:/var/empty:/usr/bin/false'
        uname, *fields, homedir, sh = line.split(':')
        self.assertEqual(uname, 'nobody')
        self.assertEqual(homedir, '/var/empty')
        self.assertEqual(sh, '/usr/bin/false')
    

        # py-ignore and star

        record = ('ACME', 50, 123.45, (12, 18, 2012))
        name, *_, (*_, year) = record
        self.assertEqual(name, 'ACME')
        self.assertEqual(year, 2012)


#={===========================================================================
# py-iterator

class TestIterator(unittest.TestCase):
    
    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    # *py-print-end*
    # Notice that the print uses end='' here to suppress adding a \n, because
    # line strings already have one (without this, our output would be
    # double-spaced; in 2.X, a trailing comma works the same as the end).

    # read all lines
    def test_iterator_on_file(self):
        f = open('trispam.txt')
        for line in f:
            print(line, end='')
        f.close()


    # how for loops internally process built-in sequence types such as lists:

    def test_iterator_protocol(self):

        coll = [1,2,3]

        # obtain an `iterator object` from an `iterable`
        it = iter(coll)

        # call iterator's next to advance to next item
        print(it.__next__())
        print(it.__next__())
        print(it.__next__())

        with self.assertRaises(StopIteration):
            print(it.__next__())

    def test_iterator_pass(self):

        # py-range do support multiple active iterators

        coll = range(3)

        it1 = iter(coll)
        it2 = iter(coll)

        self.assertEqual(next(it1), 0)
        self.assertEqual(next(it1), 1)

        self.assertEqual(next(it2), 0)

        # py-zip do not support multiple active iterators
        # By contrast, in 3.X zip, map, and filter do not support multiple
        # active iterators

        coll = zip((1,2,3), (10,11,12))

        it1 = iter(coll)
        it2 = iter(coll)

        self.assertEqual(next(it1), (1, 10))
        self.assertEqual(next(it1), (2, 11))

        self.assertEqual(next(it2), (3, 12))


    def test_iterator_on_any(self):

        f  = open('trispam.txt')
        coll = list(f)
        self.assertEqual(coll, 
                ['spam\n', 'Spam\n', 'SPAM!\n'])

        f.close()

        f  = open('trispam.txt')
        coll = tuple(f)
        self.assertEqual(coll, 
                ('spam\n', 'Spam\n', 'SPAM!\n'))

        f.close()

        f  = open('trispam.txt')
        coll = '&&'.join(f)
        self.assertEqual(coll, 
                'spam\n&&Spam\n&&SPAM!\n')

        f.close()

        f  = open('trispam.txt')
        coll = list(f)
        f.close()

        # sequence assignment
        a, b, c = coll
        self.assertEqual(a, 'spam\n')

        # *py-3* star expression
        a, *b = coll
        self.assertEqual(b, ['Spam\n', 'SPAM!\n'])

        # *py-membership*
        self.assertTrue('spam\n' in coll)

        # slice
        result = [11, 12, 13, 44]
        result[1:3] = coll
        self.assertEqual(result, 
                [11, 'spam\n', 'Spam\n', 'SPAM!\n', 44])

        # py-zip py-dict
        mapping = dict(zip(range(5), reversed(range(5))))
        self.assertEqual(mapping,
            {0: 4, 1: 3, 2: 2, 3: 1, 4: 0})


    def test_comprehension_on_any(self):

        # use py-range to loop and it not best approach
        coll = [1,2,3,4,5]

        for i in range(len(coll)):
            coll[i] += 10

        self.assertEqual(coll, [11, 12, 13, 14, 15])

        # use for
        coll = [1,2,3,4,5]
        result = []
        for i in coll:
            result.append(i + 10)

        self.assertEqual(result, [11, 12, 13, 14, 15])

        # use py-comprehension
        coll = [1,2,3,4,5]

        coll = [x + 10 for x in coll]

        self.assertEqual(coll, [11, 12, 13, 14, 15])

        # use filter condition
        coll = ['a', 'as', 'bat', 'car', 'dove', 'python']
        result = [e.upper() for e in coll if len(e) > 2]
        self.assertEqual(result, ['BAT', 'CAR', 'DOVE', 'PYTHON'])

        # nested
        coll = [x + y for x in 'abc' for y in 'lmn']
        self.assertEqual(coll, 
                ['al', 'am', 'an', 'bl', 'bm', 'bn', 'cl', 'cm', 'cn'])

        #
        coll = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
        result = [e[1] for e in coll]
        self.assertEqual(result, [2,5,8])

        # collect a diagonal from matrix
        coll = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
        result = [coll[i][i] for i in [0, 1, 2]]
        self.assertEqual(result, [1, 5, 9])

        # repeat characters in a string
        coll = [c * 2 for c in 'spam']     
        self.assertEqual(coll, ['ss', 'pp', 'aa', 'mm'])

        # multiple values, "if" filters
        coll = [[x ** 2, x ** 3] for x in range(4)]  
        self.assertEqual(coll, [[0, 0], [1, 1], [4, 8], [9, 27]])


    # py-readlines method that loads the file into a list of line strings all at
    # once:

    def test_comprehension_on_file(self):

        f  = open('trispam.txt')
        lines = f.readlines()
        self.assertEqual(lines, ['spam\n', 'Spam\n', 'SPAM!\n'])
        f.close()

        # It would be nice if we could get rid of these newlines all at once,
        # wouldn’t it?

        f  = open('trispam.txt')
        lines = f.readlines()
        result = [line.rstrip() for line in lines]
        self.assertEqual(result, ['spam', 'Spam', 'SPAM!'])
        f.close()

        # do the same
        f = open('trispam.txt')
        result = [line.rstrip() for line in f]
        self.assertEqual(result, ['spam', 'Spam', 'SPAM!'])
        f.close()

    # *py-enumerater*
    def test_iterator_enumerate(self):

        f  = open('trispam.txt')
        coll = list(enumerate(f))
        self.assertEqual(coll, 
                [(0, 'spam\n'), (1, 'Spam\n'), (2, 'SPAM!\n')])
        f.close()

        coll = ['one', 'two', 'three']
        mapping = dict((v,i) for i,v in enumerate(coll))
        self.assertEqual(mapping, {'three': 2, 'two': 1, 'one': 0})

    # *py-reversed*
    def test_iterator_reversed(self):

        coll = list(range(10))
        self.assertEqual(coll, [0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
        self.assertEqual(list(reversed(coll)), 
                [9, 8, 7, 6, 5, 4, 3, 2, 1, 0])

    # *py-map*
    def test_comprehension_on_map(self):

        f  = open('trispam.txt')
        coll = [line.upper() for line in f]
        self.assertEqual(coll, 
                ['SPAM\n', 'SPAM\n', 'SPAM!\n'])
        f.close()

        f  = open('trispam.txt')
        coll = list(map(str.upper, f))
        self.assertEqual(coll, 
                ['SPAM\n', 'SPAM\n', 'SPAM!\n'])
        f.close()

    # *py-zip* *py-3*
    def test_comprehension_on_zip(self):

        coll1 = ['foo','bar','baz']
        coll2 = ['one','two','three']
        self.assertEqual(list(zip(coll1, coll2)), 
                [('foo', 'one'), ('bar', 'two'), ('baz', 'three')])

        coll1 = ['foo','bar','baz']
        coll2 = ['one','two']
        self.assertEqual(list(zip(coll1, coll2)), 
                [('foo', 'one'), ('bar', 'two')])

        coll1 = (1, 2)
        coll2 = (3, 4)
        self.assertEqual(list(zip(coll1, coll2)), 
                [(1,3), (2,4)])

        self.assertEqual(list(zip(*zip(coll1, coll2))), 
                [(1,2), (3,4)])

        #
        coll1 = ['foo','bar','baz']
        coll2 = ['one','two','three']
        zipped = zip(coll1, coll2)

        # *py-unpack*
        names, numbers = zip(*zipped)

        self.assertEqual(names, ('foo', 'bar', 'baz'))
        self.assertEqual(numbers, ('one', 'two', 'three'))


#={===========================================================================
# py-iterator py-generator

class TestGenerator(unittest.TestCase):
    
    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    @staticmethod
    def squares(n):
        result = []
        for i in range(n):
            result.append(i ** 2)
        return result

    @staticmethod
    def gensquares(n):
        for i in range(n):
            yield i ** 2

    @staticmethod
    def ups(line):
        for sub in line.split(','):
            yield sub.upper()

    def test_generator(self):

        result = []
        for i in TestGenerator.squares(5):
            result.append(i)

        self.assertEqual(result, [0,1,4,9,16])

        result = []
        for i in TestGenerator.gensquares(5):
            result.append(i)

        self.assertEqual(result, [0,1,4,9,16])

        # iter(gen) is not required since generators are their own iterator,
        # supporting just one active iteration scan.

        gen = TestGenerator.gensquares(4)
        self.assertEqual(next(gen), 0)
        self.assertEqual(next(gen), 1)
        self.assertEqual(next(gen), 4)
        self.assertEqual(next(gen), 9)

        # 
        coll = tuple(TestGenerator.ups('aaa,bbb,ccc'))
        self.assertEqual(coll, ('AAA', 'BBB', 'CCC'))

    @staticmethod
    def frange(start, stop, increment):

        x = start

        while x < stop:
            yield x
            x += increment

    def test_cookbook_4_3(self):

        coll = [e for e in TestGenerator.frange(0, 4, 0.5)]
        self.assertEqual(coll, 
                [0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5])


#={===========================================================================
# py-function

import re

# apply str functions to remove puncuations

def clean_strings(strings):

    result = []

    for e in strings:
        e = e.strip()
        e = re.sub('[!?#]', '', e)
        e = e.title()
        result.append(e)

    return result

# py-resulable it is more resuable

def clean_strings_use_ops(strings, ops):

    result = []

    for e in strings:
        for f in ops:
            e = f(e)
        result.append(e)

    return result

def remove_punctuation(value):
    return re.sub('[!?#]', '', value)

X = 99

def selector1():
    print(X)

def selector2():
    print(X)
    X = 88

class TestFunction(unittest.TestCase):
    
    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_function_objects(self):

        states = [' Alabama ', 'Georgia!', 'FlOrIda', 
                'south carolina##', 'West virginia?']

        result = clean_strings(states)

        self.assertEqual(result, 
                ['Alabama', 'Georgia', 'Florida', 
                    'South Carolina', 'West Virginia'])

        # use list of operations
        # *py-fobj* if change remove_punctuation() then have to redefine
        # clean_ops again since clean_ops has old function address
        # 
        # defining a function with same name will create function on different
        # address

        clean_ops = [str.strip, remove_punctuation, str.title]

        result = clean_strings_use_ops(states, clean_ops)

        self.assertEqual(result, 
                ['Alabama', 'Georgia', 'Florida', 
                    'South Carolina', 'West Virginia'])

    def _times(self, x, y):
        return x * y

    def _intersect(self, coll1, coll2):
        result = []

        for e in coll1:
            if e in coll2:
                result.append(e)

        return result

    # py-argument

    def _f(self, a):
        a = 99

    def _changer(self, a, b):
        a = 2
        b[0] = 'spam'

    def test_function_argument(self):

        b = 88

        self._f(b)

        # expected the change of b?
        self.assertEqual(b, 88)

        X = 1
        L = [1, 2]
        self._changer(X, L)

        # expected the change of L?
        self.assertEqual(L, ['spam', 2])


    def test_function_argument_type(self):

        result = self._times(2, 4)
        self.assertEqual(result, 8)

        result = self._times(3.14, 4)
        self.assertEqual(result, 12.56)

        result = self._times('Ni', 4)
        self.assertEqual(result, 'NiNiNiNi')

        # see that TypeError gets raised from _times()
        #
        # Traceback (most recent call last):
        #   File "/home/kyoupark/git/kb/code-py/pybase/pycore.py", line 1778, in test_function_arg_type
        #     result = self._times('Ni', 'Pi')
        #   File "/home/kyoupark/git/kb/code-py/pybase/pycore.py", line 1765, in _times
        #     return x * y
        # TypeError: can't multiply sequence by non-int of type 'str'

        with self.assertRaises(TypeError):
            result = self._times('Ni', 'Pi')

        #
        result = self._intersect([1, 2, 3], (1, 4))
        self.assertEqual(result, [1])


    #   File "/home/kyoupark/git/kb/code-py/pybase/pycore.py", line 1737, in selector2
    #     print(X)
    # UnboundLocalError: local variable 'X' referenced before assignment

    def test_function_local_namespace(self):
        selector1()

        with self.assertRaises(UnboundLocalError):
            selector2()

    # py-closure

    def _make_closure(self, value):

        def closure():
            return 'value is ' + str(value)

        return closure

    def test_function_closure_1(self):
        clo = self._make_closure(5)

        self.assertEqual(clo(), 'value is 5')
        self.assertEqual(clo(), 'value is 5')
        self.assertEqual(clo(), 'value is 5')


    def _make_counter(self):
        count = [0]

        def counter():
            count[0] += 1
            return count[0]

        return counter

    def test_function_closure_2(self):

        clo = self._make_counter()

        self.assertEqual(clo(), 1)
        self.assertEqual(clo(), 2)
        self.assertEqual(clo(), 3)


    def _format_and_pad(self, format, space):
        def formatter(x):
            return (format % x).rjust(space)
        return formatter

    def test_function_closure_3(self):

        clo = self._format_and_pad('%.4f', 15)

        self.assertEqual(clo(1.756), '         1.7560')
        self.assertEqual(clo(1.7), '         1.7000')
        self.assertEqual(clo(1), '         1.0000')


    # py-lambda

    def _apply_to_list(self, coll, f):
        return [f(e) for e in coll]

    def test_function_lambda(self):

        coll = [4,0,1,5,6]
        result = self._apply_to_list(coll, lambda x: x * 2)
        self.assertEqual(result, 
                [8, 0, 2, 10, 12])

    # py-variable-arg

    def _print_variable_args(self, *arg, **karg):
        print(arg, karg)

    # [RUN]  test_function_vaarg
    # () {}
    # (1,) {}
    # (1, 2, 3) {}
    # (1, 2, 3) {'a': 1, 'b': 2}

    def test_function_vaarg_1(self):            
        self._print_variable_args()
        self._print_variable_args(1)
        self._print_variable_args(1,2,3)
        self._print_variable_args(1,2,3, a = 1, b = 2)


    def _print_variable_tuple(self, a, b, c, d):
        print(a, b, c, d)

    # [RUN]  test_function_vaarg_2
    # (1, 2, 3, 4) {}
    # 1 2 3 4
    # 1 2 3 4

    def test_function_vaarg_2(self):            
        coll  = (1,2,3,4)
        self._print_variable_args(*coll)
        self._print_variable_tuple(*coll)

        coll = {'a':1, 'b':2, 'c':3, 'd':4}
        # same as func(a=1, b=2, c=3, d=4)
        self._print_variable_tuple(**coll)


#={===========================================================================
# py-exception py-with

class AlreadyGotOne(Exception):
    pass

# Notice that this class’s __exit__ method returns False to propagate the
# exception; deleting the return statement would have the same effect, as the
# default None return value of functions is False by definition. Also notice
# that the __enter__ method returns self as the object to assign to the as
# variable; in other use cases, this might return a completely different object
# instead.

class TraceBlock:
    def message(self, arg):
        print('running ' + arg)

    def __enter__(self):
        print('starting with block')
        return self

    def __exit__(self, exc_type, exc_value, exc_tb):
        if exc_type is None:
            print('exited normally\n')
        else:
            print('raise an exception!' + str(exc_type))
            # propagate
            return False

class TestException(unittest.TestCase):
    
    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def _fetcher(self, obj, index):
        return obj[index]

    def test_exception_default_handler(self):

        self.assertEqual(self._fetcher('spam', 3), 'm')

        # py-exception-default-handler
        #
        # >>> fetcher(x, 4) 
        # Traceback (most recent call last):
        # File "<stdin>", line 1, in <module>
        # File "<stdin>", line 2, in fetcher
        # IndexError: string index out of range

        # default handler of unittest
        #
        # ======================================================================
        # ERROR: test_exception_default_handler (pycore.TestException)
        # ----------------------------------------------------------------------
        # Traceback (most recent call last):
        #   File "/home/kyoupark/git/kb/code-py/pybase/pycore.py", line 1935, in test_exception_default_handler
        #     self.assertEqual(self._fetcher('spam', 4), 'm')
        #   File "/home/kyoupark/git/kb/code-py/pybase/pycore.py", line 1929, in _fetcher
        #     return obj[index]
        # IndexError: string index out of range

        # self.assertEqual(self._fetcher('spam', 4), 'm')

        # now handles exception

        try:
            # trigger manually
            # raise IndexError
            self.assertEqual(self._fetcher('spam', 4), 'm')
        except IndexError:
            print('got exception')

        print('continuing')


    def _grail(self):
        raise AlreadyGotOne()

    def test_exception_user_defined(self):

        try:
            self._grail()
        except AlreadyGotOne:
            print('got exception')


    def _after(self):

        try:
            self._fetcher('spam', 4)
        finally:
            print('after fetch')
        print('after try?')

    def test_exception_termination(self):
        
        with self.assertRaises(IndexError):
            self._after()


    def test_exception_context_manager(self):

        # there is warning if not call close()

        f  = open('trispam.txt')
        coll = list(f)
        self.assertEqual(coll, 
                ['spam\n', 'Spam\n', 'SPAM!\n'])
        f.close()

        # general and explicit try/finally statement, but it requires three more
        # lines of administrative code

        f  = open('trispam.txt')
        try:
            coll = list(f)
            self.assertEqual(coll, 
                    ['spam\n', 'Spam\n', 'SPAM!\n'])
        finally:
            f.close()

        # file objects have a context manager that automatically closes the file
        # after the with block regardless of whether an exception is raised,
        
        with open('trispam.txt') as f:
            cpll = list(f)
            self.assertEqual(coll, 
                    ['spam\n', 'Spam\n', 'SPAM!\n'])

    # [RUN]  test_exception_context_manager_user
    # starting with block
    # running test 1
    # reached
    # exited normally
    # 
    # starting with block
    # running test 2
    # raise an exception!<class 'TypeError'>

    def test_exception_context_manager_user(self):

        with TraceBlock() as action:
            action.message('test 1')
            print('reached')

        with TraceBlock() as action:
            action.message('test 2')
            raise TypeError
            print('not reached')


#={===========================================================================
# py-exception py-cookbook_8_3

# PYCB3, 8.3. Making Objects Support the Context-Management Protocol
#
# Problem
#
# You want to make your objects support the context-management protocol (the
# with statement).

from socket import socket, AF_INET, SOCK_STREAM
from functools import partial

class LazyConnection:

    # *py-error* see that "__init___" has more _ !
    # TypeError: object() takes no parameters
    # def __init___(self, address, family = AF_INET, type = SOCK_STREAM):

    def __init__(self, address, family = AF_INET, type = SOCK_STREAM):
        self.address = address
        self.family = AF_INET
        self.type = SOCK_STREAM
        self.sock = None

    def __enter__(self):
        print('__enter__ is called')
        if self.sock is not None:
            raise RuntimeError('Alreay connected')
        self.sock = socket(self.family, self.type)
        self.sock.connect(self.address)
        return self.sock

    def __exit__(self, exc_ty, exc_val, tb):
        print('__exit__ is called')
        self.sock.close()
        self.sock = None

class TestExceptionContext(unittest.TestCase):
    
    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_coookbook_8_3(self):

        conn = LazyConnection(('www.python.org', 80))

        with conn as s:
            # conn.__enter__() executes: connection open
            s.send(b'GET /index.html HTTP/1.0\r\n')
            s.send(b'Host: www.python.org\r\n')
            s.send(b'\r\n')
            resp = b''.join(iter(partial(s.recv, 8192), b''))
            # conn.__exit__() executes: connection closed


#={===========================================================================
# py-reverse

class TestReverse(unittest.TestCase):
    
    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_reverse(self):

        # py-list sort
        coll = ['abc', 'ABD', 'aBe']
        coll.sort(key = str.lower, reverse = True)
        self.assertEqual(coll, ['aBe', 'ABD', 'abc'])

        # py-slice
        # A clever use of this is to pass -1 which has the useful effect of
        # reversing a list or tuple:

        coll = [7, 2, 3, 6, 3, 5, 6, 0, 1]
        self.assertEqual(coll[::-1], [1, 0, 6, 5, 3, 6, 3, 2, 7])

        # *py-reversed*
        # `reversed` iterates over the elements of a sequence in reverse order:
        coll = list(range(10))
        self.assertEqual(coll, [0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
        self.assertEqual(list(reversed(coll)), 
                [9, 8, 7, 6, 5, 4, 3, 2, 1, 0])


#={===========================================================================
# py-subprocess

class TestSubprocess(unittest.TestCase):
    
    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_get_output_from_subprocess(self):

        try:
            p = subprocess.Popen(["pwd"], stdout=subprocess.PIPE)
        except OSError:
            return None

        result = p.communicate()

        # In Python3:
        # Bytes literals are always prefixed with 'b' or 'B'; they produce an
        # instance of the bytes type instead of the str type. They may only
        # contain ASCII characters; bytes with a numeric value of 128 or greater
        # must be expressed with escapes.

        self.assertEqual(result, (b'/home/keitee/git/kb/code-py/pybase\n', None))

    def test_get_output_from_subprocess_and_process(self):

        gitcmds = r'git show :./some.txt'

        # make it list to run in Popen()
        cmds = gitcmds.split()

        try:
            p = subprocess.Popen(cmds, stdout=subprocess.PIPE)
        except OSError:
            return None

        # returns a tuple
        result = p.communicate()

        # result[0] is a string which has multiple lines with line breaks. so
        # have to make it to list

        print(result[0])

        output = str(result[0]).splitlines()
        print(output)


        # when runs on python3
        #
        # >>> result[0]
        # b'<?xml version="1.0" encoding="UTF-8"?>\n<ivy-module version="2.0">\n  <info organisation="middleware" module="bin_aem_bin"/>\n       <dependencies>\n<dependency org="middleware" name="aem_bin" rev="AEM_F76.3.REL_AEM_MAIN_2_Integration">\n<artifact name="aem_bin" ext="tar.gz" type="gz"/>\n        </dependency>\n        </dependencies>\n</ivy-module>\n'
        #
        # >>> str(result[0])
        # 'b\'<?xml version="1.0" encoding="UTF-8"?>\\n<ivy-module version="2.0">\\n  <info organisation="middleware" module="bin_aem_bin"/>\\n       <dependencies>\\n<dependency org="middleware" name="aem_bin" rev="AEM_F76.3.REL_AEM_MAIN_2_Integration">\\n<artifact name="aem_bin" ext="tar.gz" type="gz"/>\\n        </dependency>\\n        </dependencies>\\n</ivy-module>\\n\''
        #
        # >>> str(result[0]).splitlines()
        # ['b\'<?xml version="1.0" encoding="UTF-8"?>\\n<ivy-module version="2.0">\\n  <info organisation="middleware" module="bin_aem_bin"/>\\n       <dependencies>\\n<dependency org="middleware" name="aem_bin" rev="AEM_F76.3.REL_AEM_MAIN_2_Integration">\\n<artifact name="aem_bin" ext="tar.gz" type="gz"/>\\n        </dependency>\\n        </dependencies>\\n</ivy-module>\\n\'']
        #
        # >>> len(str(result[0]).splitlines())
        # 1

        # when runs on python2 
        #
        # >>> result[0]
        # '<?xml version="1.0" encoding="UTF-8"?>\n<ivy-module version="2.0">\n  <info organisation="middleware" module="bin_aem_bin"/>\n       <dependencies>\n<dependency org="middleware" name="aem_bin" rev="AEM_F76.3.REL_AEM_MAIN_2_Integration">\n<artifact name="aem_bin" ext="tar.gz" type="gz"/>\n        </dependency>\n        </dependencies>\n</ivy-module>\n'
        # 
        # >>> str(result[0])
        # '<?xml version="1.0" encoding="UTF-8"?>\n<ivy-module version="2.0">\n  <info organisation="middleware" module="bin_aem_bin"/>\n       <dependencies>\n<dependency org="middleware" name="aem_bin" rev="AEM_F76.3.REL_AEM_MAIN_2_Integration">\n<artifact name="aem_bin" ext="tar.gz" type="gz"/>\n        </dependency>\n        </dependencies>\n</ivy-module>\n'
        # 
        # >>> str(result[0]).splitlines()
        # ['<?xml version="1.0" encoding="UTF-8"?>', '<ivy-module version="2.0">', '  <info organisation="middleware" module="bin_aem_bin"/>', '       <dependencies>', '<dependency org="middleware" name="aem_bin" rev="AEM_F76.3.REL_AEM_MAIN_2_Integration">', '<artifact name="aem_bin" ext="tar.gz" type="gz"/>', '        </dependency>', '        </dependencies>', '</ivy-module>']
        #
        # >>> len(str(result[0]).splitlines())
        # 9

        for line in output:
            print(line)


#={===========================================================================
if __name__ == '__main__':
    unittest.main()
