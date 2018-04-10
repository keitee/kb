import unittest

class TestDictCreation(unittest.TestCase):

    # create dict by assign

    # The first is handy if you can spell out the entire dictionary ahead of
    # time.

    def test_create_by_assign(self):
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

    def test_sort_dict(self):
        d = {'a':100, 'c':300, 'b':200}

        for key in d:
            print(key, '=>', d[key])

        print('--------------')

        for key in sorted(d):
            print(key, '=>', d[key])

class TestDictDefaultValue(unittest.TestCase):

    def test_dict_get(self):
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

# py-list
class TestStringSplitMethods(unittest.TestCase):

    def test_split(self):
        s = 'hello world'
        self.assertEqual(s.split(), ['hello', 'world'])
        # check that s.split fails when the separator is not a string
        with self.assertRaises(TypeError):
            s.split(2)

if __name__ == '__main__':
    unittest.main()
