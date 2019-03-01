import unittest

# $ python coll_test.py --verbose
#
# to run single test
# $ python -m unittest coll_test.TestDictCreation.test_create_by_assign
# 
# to run all of a class
# $ python -m unittest coll_test.TestDictCreation

#={===========================================================================
# py-xxx

# LPY5, Chapter 32: Advanced Class Topics
# Extending Types by Embedding

class Set:
    def __init__(self, value = []):
        self.data = []
        self.concat(value)

    # common(self + other)
    def intersect(self, other):
        res = []
        for e in self.data:
            if e in other:
                res.append(e)
        return Set(res)

    # self + other
    # res is list so cannot use concat()
    def union(self, other):
        res = self.data[:]
        for e in other:
            if not e in res:
                res.append(e)
        return Set(res)

    # value can be T. remove duplicates by adding items which are in in data.
    def concat(self, value):
        for e in value:
            if not e in self.data:
                self.data.append(e)

    # len(self)
    def __len__(self):
        # print('__len__')
        return len(self.data)

    # self[i], self[i:j]
    def __getitem__(self, key):
        # print('__getitem__')
        return self.data[key]

    # self & other
    def __and__(self, other):
        # print('__and__')
        return self.intersect(other)

    # self | other
    def __or__(self, other):
        # print('__or__')
        return self.union(other)

    def __repr__(self):
        # print('__repr__')
        return 'Set: ' + repr(self.data)

    # when not defines __iter__(), calls __getitem__() instead and more times
    def __iter__(self):
        # print('__iter__')
        return iter(self.data)


class TestMySet(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN] ", self._testMethodName


    def test_extend_by_composite(self):
        x = Set([1,3,5,7])
        y = Set([1,2,3,4,5,6])
        print(x, y, len(x))
        print(x.intersect(y))
        print(x & y)
        print(x.union(y))
        print(x | y)

        # x.reverse()
        # print(x)


# Extending Types by Subclassing

class MyList(list):
    def __getitem__(self, offset):
        print('(indexing %s at %s)' % (self, offset))
        return list.__getitem__(self, offset-1)

class TestMyList(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN] ", self._testMethodName

    # [RUN]  test_mylist
    # ['a', 'b', 'c']
    # ['a', 'b', 'c']
    # (indexing ['a', 'b', 'c'] at 1)
    # a
    # (indexing ['a', 'b', 'c'] at 3)
    # c
    # ['a', 'b', 'c', 'spam']

    def test_extend_by_subclass_01(self):
        print(list('abc'))
        x = MyList('abc')
        print(x)

        print(x[1])
        print(x[3])

        x.append('spam')
        print(x)


# Extending Types by Subclassing

class SubSet(list):
    def __init__(self, value = []):

        # *py-error* cause infinite recursion
        # self.__init__([])

        list.__init__([])
        self.concat(value)

    # common(self + other)
    def intersect(self, other):
        res = []
        for e in self:
            if e in other:
                res.append(e)
        return SubSet(res)

    # self + other
    # since res is SubSet and can call concat() on it. 
    def union(self, other):
        res = SubSet(self)
        res.concat(other)
        return res

    # value can be T. remove duplicates by adding items which are in in data.
    def concat(self, value):
        for e in value:
            if not e in self:
                self.append(e)

    # self & other
    def __and__(self, other):
        # print('__and__')
        return self.intersect(other)

    # self | other
    def __or__(self, other):
        # print('__or__')
        return self.union(other)

    def __repr__(self):
        # print('__repr__')
        return 'Set: ' + list.__repr__(self)

class TestSubSet(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN] ", self._testMethodName

    # .====================
    # [RUN]  test_extend_by_composite
    # (Set: [1, 3, 5, 7], Set: [1, 2, 3, 4, 5, 6], 4)
    # Set: [1, 3, 5]
    # Set: [1, 3, 5]
    # Set: [1, 3, 5, 7, 2, 4, 6]
    # Set: [1, 3, 5, 7, 2, 4, 6]
    # .====================
    # [RUN]  test_extend_by_subclass_02
    # (Set: [1, 3, 5, 7], Set: [1, 2, 3, 4, 5, 6], 4)
    # Set: [1, 3, 5]
    # Set: [1, 3, 5]
    # Set: [1, 3, 5, 7, 2, 4, 6]
    # Set: [1, 3, 5, 7, 2, 4, 6]
    # Set: [7, 5, 3, 1]

    def test_extend_by_subclass_02(self):
        x = SubSet([1,3,5,7])
        y = SubSet([1,2,3,4,5,6])
        print(x, y, len(x))
        print(x.intersect(y))
        print(x & y)
        print(x.union(y))
        print(x | y)

        x.reverse()
        print(x)


if __name__ == '__main__':
    unittest.main()
