import unittest

# $ python coll_test.py --verbose
#
# to run single test
# $ python -m unittest coll_test.TestDictCreation.test_create_by_assign
# 
# to run all of a class
# $ python -m unittest coll_test.TestDictCreation

class TestDictCreation(unittest.TestCase):

    # create dict by assign

    # The first is handy if you can spell out the entire dictionary ahead of
    # time.

    # note: name should start with "test_" and otherwise, will not be run.

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

        print

        d = {'name': 'Bob', 'age': 40}
        self.assertEqual( d, {'name': 'Bob', 'age': 40})


    # create dict by keyword argument form

    # The third involves less typing than the first, but it requires all keys to
    # be strings.

    def test_create_by_keyword_form(self):

        print

        d = dict(name='Bob', age=40)
        self.assertEqual( d, {'name': 'Bob', 'age': 40})

    # create dict by key, value tuple form

    # The last is useful if you need to build up keys and values as sequences at
    # runtime.

    def test_create_by_tuple_form(self):

        print

        d = dict([('name', 'Bob'), ('age', 40)])
        self.assertEqual( d, {'name': 'Bob', 'age': 40})


class TestDictSort(unittest.TestCase):

    # def setUp(self):
    #     print "In method", self._testMethodName

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
            print 'exist'
        else: 
            print 'not exist'

class TestDictInsertAndErase(unittest.TestCase):

    # test_pop_and_del (__main__.TestDictInsertAndErase) ...
    # {'a': 'some value', 'dummy': 'another value', 'b': [1, 2, 3, 4], 5: 'some value', 7: 'an integer'}
    # {'a': 'some value', 'dummy': 'another value', 'b': [1, 2, 3, 4], 7: 'an integer'}
    # {'a': 'some value', 'b': [1, 2, 3, 4], 7: 'an integer'}
    # ok

    def test_pop_and_del(self):
        print

        d = {'a': 'some value', 'dummy': 'another value', 'b': [1, 2, 3, 4], 5: 'some value', 7: 'an integer'}
        print d

        del d[5]
        print d

        d.pop('dummy')
        print d


class TestDictMemberFunctions(unittest.TestCase):

    # test_use_keys_and_values (__main__.TestDictMemberFunctions) ...
    # ['a', 'dummy', 'b', 5, 7]
    # ['some value', 'another value', [1, 2, 3, 4], 'some value', 'an integer']
    # ok

    def test_use_keys_and_values(self):
        print

        d = {'a': 'some value', 'dummy': 'another value', 'b': [1, 2, 3, 4], 5: 'some value', 7: 'an integer'}

        print d.keys()
        print d.values()

    # note: b is overwritten
    #
    # test_use_update (__main__.TestDictMemberFunctions) ...
    # {'a': 'some value', 'b': [1, 2, 3, 4], 7: 'an integer'}
    # {'a': 'some value', 'c': 12, 'b': 'foo', 7: 'an integer'}
    # ok

    def test_use_update(self):
        print

        d = {'a': 'some value', 'b': [1, 2, 3, 4], 7: 'an integer'}
        print d

        d.update({'b':'foo','c':12})
        print d

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
        print table.items()

        # *list-comprehension*
        print [title for (title, year) in table.items() if year == '1975']


class TestDictDefaultValue(unittest.TestCase):

    # test_dict_basic (__main__.TestDictDefaultValue) ...
    # when found: a is      100
    # when not found: d is  None
    # when not found: d is  400
    # ok

    def test_dict_basic(self):
        print

        d = {'a':100, 'c':300, 'b':200}

        print "when found: a is     " , d.get('a', 400)
        print "when not found: d is " , d.get('d')
        print "when not found: d is " , d.get('d', 400)


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
        print 'egrep -an "' + '|'.join(result) + '"'

        # for (handle, occurance) in d.items():
        #     if occurance == 1:
        #         print handle

        print d

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

        print by_letter

    # the same result. Q: how does the first word get added?

    def test_dict_use_get_to_set_default_value(self):
        print

        words=['apple', 'bat', 'bar', 'atom', 'book']

        by_letter={}

        for word in words:
            letter = word[0]
            by_letter.setdefault(letter, []).append(word)

        print by_letter

# py-list
class TestStringSplitMethods(unittest.TestCase):

    def test_split(self):

        print

        s = 'hello world'
        self.assertEqual(s.split(), ['hello', 'world'])
        # check that s.split fails when the separator is not a string
        with self.assertRaises(TypeError):
            s.split(2)

if __name__ == '__main__':
    unittest.main()
