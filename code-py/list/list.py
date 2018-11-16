import unittest


class TestList(unittest.TestCase):

    def test_indexing_slicing(self):
        L = ['spam', 'Spam', 'SPAM!']

        # Offsets start at zero
        self.assertEqual( L[2], 'SPAM!')

        # Negative: count from the right
        self.assertEqual( L[-2], 'Spam')

        """ 
        slicing retuns a list

        Because lists are sequences, indexing and slicing work the same way for
        lists as they do for strings. However, the result of indexing a list is
        whatever type of object lives at the offset you specify, while slicing a
        list always `returns a new list`.

        """
        self.assertEqual( L[1:], ['Spam', 'SPAM!'])


    def test_comprehension(self):
        strings = ['a', 'as', 'bat', 'car', 'dove', 'python']
        results = [x.upper() for x in strings if len(x) > 2 ]
        self.assertEqual( results, ['BAT', 'CAR', 'DOVE', 'PYTHON'])


if __name__ == '__main__':
    unittest.main()
