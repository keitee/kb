import unittest

"""
$ python coll_test.py --verbose

1. to run single test
$ python -m unittest coll_test.TestDictCreation.test_create_by_assign

   to run single test set
$ python -m unittest pycore.TestList

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

        self.assertEqual(3 / 2, 1)
        self.assertEqual(3 / float(2), 1.5)

        self.assertEqual(21600/90000, 0)
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

    def test_string_performance(self):

        # to list
        coll = 'spammy'
        l = list(coll)

        l[4] = l[3] = 'x'

        # to string back
        coll = ''.join(l)
        self.assertEqual(coll, 'spaxxy')


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


    def test_list_range(self):

        # py-range
        coll = range(10)
        self.assertEqual(coll, [0, 1, 2, 3, 4, 5, 6, 7, 8, 9])

        #
        coll2 = [0] * 4;
        coll3 = [5,6,7,8]

        self.assertEqual(coll2, [0, 0, 0, 0])

        for i in range(len(coll2)):
            coll2[i] = coll3[i]

        self.assertEqual(coll2, coll3)

        # py-modulo
        sum = 0
        for i in xrange(10000):

            # py-way no precedence between ops?
            if i % 3 == 0 or i % 5 == 0:
                sum += i

        self.assertEqual(sum, 23331668)


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

        coll = ['spam', 'eggs', 'ham', 'toast']
        coll.pop(2)
        self.assertEqual(coll, 
                ['spam', 'eggs', 'toast'])

        # py-list-remove
        # search the first and remove it

        coll = ['foo', 'red', 'red', 'red', 'baz', 'dwarf']
        coll.remove('red')
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


    def test_list_comprehension(self):
        coll = ['a', 'as', 'bat', 'car', 'dove', 'python']
        result = [e.upper() for e in coll if len(e) > 2]
        self.assertEqual(result, ['BAT', 'CAR', 'DOVE', 'PYTHON'])


    # [RUN]  test_list_elements
    # ['l', 'i', 's', 't', 1, 2, 3, 'type']

    def test_list_elements(self):
        coll = list('list')
        coll.append(1)
        coll.append(2)
        coll.append(3)
        coll.append('type')
        print(coll)


#={===========================================================================
# py-dict

class TestDictCreation(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    # create dict by assign
    # The first is handy if you can spell out the entire dictionary ahead of
    # time.

    def test_create_by_assign(self):

        # to print a empty line. why?
        # print

        d = {}
        d['name'] = 'Bob'
        d['age'] = 40
        self.assertEqual( d, {'name': 'Bob', 'age': 40})


    # create dict by initializers
    # The second is of use if you need to create the dictionary one field at a
    # time on the fly.

    def test_create_by_initializers(self):

        d = {'name': 'Bob', 'age': 40}
        self.assertEqual( d, {'name': 'Bob', 'age': 40})


    # create dict by keyword argument form
    # The third involves less typing than the first, but it requires all keys to
    # be strings.

    def test_create_by_keyword_form(self):

        d = dict(name='Bob', age=40)
        self.assertEqual( d, {'name': 'Bob', 'age': 40})

    # create dict by key, value tuple form
    # The last is useful if you need to build up keys and values as sequences at
    # runtime.

    def test_create_by_tuple_form(self):

        d = dict([('name', 'Bob'), ('age', 40)])
        self.assertEqual( d, {'name': 'Bob', 'age': 40})


class TestDictSort(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    # ('a', '=>', 100)
    # ('c', '=>', 300)
    # ('b', '=>', 200)
    # --------------
    # ('a', '=>', 100)
    # ('b', '=>', 200)
    # ('c', '=>', 300)

    # in recent versions of Python it can be done in one step with the newer
    # `sorted()` built-in function.

    def test_sort_dict_not_use_member(self):

        print

        d = {'a':100, 'c':300, 'b':200}

        # same as 'for key in d.keys():'
        for key in d:
            print(key, '=>', d[key])

        print('--------------')

        for key in sorted(d):
            print(key, '=>', d[key])

    def test_sort_dict_use_member(self):

        print

        d = {'a':100, 'c':300, 'b':200}

        for key in d:
            print(key, '=>', d[key])

        key_list = list(d.keys())
        key_list.sort()

        print('--------------')

        for key in key_list:
            print(key, '=>', d[key])

class TestDictIteration(unittest.TestCase):

    def test_show_return_value_when_found(self):

        print

        d = {'a':100, 'c':300, 'b':200}

        if 'a' in d:
            print('exist')
        else: 
            print('not exist')

class TestDictInsertAndErase(unittest.TestCase):

    # test_pop_and_del (__main__.TestDictInsertAndErase) ...
    # {'a': 'some value', 'dummy': 'another value', 'b': [1, 2, 3, 4], 5: 'some value', 7: 'an integer'}
    # {'a': 'some value', 'dummy': 'another value', 'b': [1, 2, 3, 4], 7: 'an integer'}
    # {'a': 'some value', 'b': [1, 2, 3, 4], 7: 'an integer'}
    # ok

    def test_pop_and_del(self):
        print

        d = {'a': 'some value', 'dummy': 'another value', 'b': [1, 2, 3, 4], 5: 'some value', 7: 'an integer'}
        print(d)

        del d[5]
        print(d)

        d.pop('dummy')
        print(d)


class TestDictMemberFunctions(unittest.TestCase):

    # test_use_keys_and_values (__main__.TestDictMemberFunctions) ...
    # ['a', 'dummy', 'b', 5, 7]
    # ['some value', 'another value', [1, 2, 3, 4], 'some value', 'an integer']
    # ok

    def test_use_keys_and_values(self):
        print

        d = {'a': 'some value', 'dummy': 'another value', 'b': [1, 2, 3, 4], 5: 'some value', 7: 'an integer'}

        print(d.keys())
        print(d.values())

    # note: b is overwritten
    #
    # test_use_update (__main__.TestDictMemberFunctions) ...
    # {'a': 'some value', 'b': [1, 2, 3, 4], 7: 'an integer'}
    # {'a': 'some value', 'c': 12, 'b': 'foo', 7: 'an integer'}
    # ok

    def test_use_update(self):
        print

        d = {'a': 'some value', 'b': [1, 2, 3, 4], 7: 'an integer'}
        print(d)

        d.update({'b':'foo','c':12})
        print(d)

    # items()
    # Return a copy of the dictionary's list of (key, value) pairs.
    # searching through sequences like this is generally much slower 
    # than a direct key index

    # [('The Meaning of Life', '1983'), ('Life of Brian', '1979'), ('Holy Grail', '1975')]
    # ['Holy Grail']

    def test_uset_items(self):
        print

        # Key, Value
        table = {'Holy Grail': '1975', 'Life of Brian': '1979','The Meaning of Life': '1983'}
        print(table.items())

        # *list-comprehension*
        print([title for (title, year) in table.items() if year == '1975'])


class TestDictDefaultValue(unittest.TestCase):

    # test_dict_basic (__main__.TestDictDefaultValue) ...
    # when found: a is      100
    # when not found: d is  None
    # when not found: d is  400
    # ok

    def test_dict_basic(self):
        print

        d = {'a':100, 'c':300, 'b':200}

        print("when found: a is     " , d.get('a', 400))
        print("when not found: d is " , d.get('d'))
        print("when not found: d is " , d.get('d', 400))


    # test_dict_get (__main__.TestDictDefaultValue) ...
    # egrep -an "0x145|0x345"
    # {'0x145': 1, '0x678': 2, '0x123': 2, '0x345': 1}
    # ok

    def test_dict_get(self):
        print

        d = {}

        d['0x123'] = d.get('0x123', 0)+1
        d['0x123'] = d.get('0x123', 0)+1
        d['0x345'] = d.get('0x345', 0)+1
        d['0x678'] = d.get('0x678', 0)+1
        d['0x678'] = d.get('0x678', 0)+1
        d['0x145'] = d.get('0x145', 0)+1

        result = [handle for (handle, occurance) in d.items() if occurance == 1]
        print('egrep -an "' + '|'.join(result) + '"')

        # for (handle, occurance) in d.items():
        #     if occurance == 1:
        #         print handle

        print(d)

    # group words by the first char of a word

    # test_dict_use_default_value (__main__.TestDictDefaultValue) ...
    # {'a': ['apple', 'atom'], 'b': ['bat', 'bar', 'book']}
    # ok

    def test_dict_use_default_value(self):
        print

        words=['apple', 'bat', 'bar', 'atom', 'book']

        by_letter={}

        for word in words:
            letter = word[0]
            if letter not in by_letter:
                by_letter[letter] = [word]
            else:
                by_letter[letter].append(word)

        print(by_letter)

    # the same result. Q: how does the first word get added?

    def test_dict_use_get_to_set_default_value(self):
        print

        words=['apple', 'bat', 'bar', 'atom', 'book']

        by_letter={}

        for word in words:
            letter = word[0]
            by_letter.setdefault(letter, []).append(word)

        print(by_letter)

class TestDict(unittest.TestCase):

    def test_zip(self):
        mapping = dict(zip(range(5), reversed(range(5))))
        self.assertEqual(mapping,
            {0: 4, 1: 3, 2: 2, 3: 1, 4: 0})


#={===========================================================================
# py-coll-deque

from collections import deque

class TestCollDeque(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    @staticmethod
    def search(lines, pattern, history = 5):
        previous_lines = deque(maxlen = history)

        for line in lines:

            if pattern in line:
                yield line, previous_lines

            previous_lines.append(line)

    def test_coll_deque(self):
        with open('pycore.py') as f:
            for line, previous_lines in TestCollDeque.search(f, 'python', 5):
                for pline in previous_lines:
                    print(pline, end='')
                print(line, end='')
                print('-'*20)


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

        # from right, end()
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


#={===========================================================================
# py-base

class TestBase(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_pass(self):
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

    def test_iter(self):
        self.assertTrue(self.isiterable('string'))
        self.assertTrue(self.isiterable([1,2,3]))
        self.assertFalse(self.isiterable(4))


    def test_is(self):

        a = [1,2,3]
        b = a
        c = list(a)
        d = [1,2,3]

        # since py-reference
        self.assertTrue(a is b)
        self.assertTrue(a == b)

        # c is a different list
        self.assertTrue(a is not c)

        # same value
        self.assertTrue(a == d)

        # but different object
        self.assertFalse(a is d)

        # py-comparison
        coll1 = [1, ('a', 3)]
        coll2 = [1, ('a', 2)]
        # Less, equal, greater: tuple of results
        result = coll1 < coll2, coll1 == coll2, coll1 > coll2
        self.assertEqual(result, (False, False, True))

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

    def test_compare(self):
        version1 = '40.65.00'
        version2 = '40.66.00'
        version3 = '40.66.00'

        self.assertEqual(self.compare_version_1(version1, version2), -1)
        self.assertEqual(self.compare_version_1(version2, version1), 1)
        self.assertEqual(self.compare_version_1(version2, version3), 0)

        self.assertEqual(self.compare_version_2(version1, version2), -1)
        self.assertEqual(self.compare_version_2(version2, version1), 1)
        self.assertEqual(self.compare_version_2(version2, version3), 0)


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

    def test_zip(self):
        coll1 = ['foo','bar','baz']
        coll2 = ['one','two','three']
        self.assertEqual(zip(coll1, coll2), 
                [('foo', 'one'), ('bar', 'two'), ('baz', 'three')])

    def test_zip_different_size(self):
        coll1 = ['foo','bar','baz']
        coll2 = ['one','two']
        self.assertEqual(zip(coll1, coll2), 
                [('foo', 'one'), ('bar', 'two')])



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

    def test_iterator_on_file(self):
        for line in open('trispam.txt'):
            print(line, end='')


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


#={===========================================================================
if __name__ == '__main__':
    unittest.main()
