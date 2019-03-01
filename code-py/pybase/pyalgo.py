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
# py-leetcode-001, two sum

def two_sum(nums, target):

    myHash = {}

    for index, value in enumerate(nums):
        if target - value in myHash:
            return [myHash[target-value], index]
        myHash[value] = index

#={===========================================================================
# py-leetcode-005, LongestCommonPrefix
# from leetcode online comments

def LongestCommonPrefix(strs):
    """
    :type strs: List[str]
    :rtype: str
    """
    if not strs: return ""
    if len(strs) == 1: return strs[0]

    strs.sort()
    p = ""

    # why only compare two in the list? the reason is sort()
    #
    # once sort() is done, elements between two * are the same when * == * so
    # don't need to inspect
    #
    # x x x x x * x x x, str1
    # ...
    # x x x x x * x x x, strn

    for x, y in zip(strs[0], strs[-1]):

        # print(strs[0], strs[-1])

        if x == y: p+=x
        else: break

    return p


#={===========================================================================

class TestAlgo(unittest.TestCase):

    def setUp(self):
        print("====================")
        print("[RUN] ", self._testMethodName)

    def test_algo_leetcode_001(self):

        coll = two_sum([2,7,11,15], 9)
        self.assertEqual(coll, [0,1])

        coll = two_sum([0,4,3,0], 0)
        self.assertEqual(coll, [0,3])

        coll = two_sum([-1,-2,-3,-4,-5], -8)
        self.assertEqual(coll, [2,4])

    def test_algo_longest_common_prefix(self):
        coll = LongestCommonPrefix(["flower","flow","flight"])
        # coll = LongestCommonPrefix(["flower", "flight", "fjord", "flow", "broad"])
        self.assertEqual(coll, "fl")

        self.assertEqual(LongestCommonPrefix(["dog","racecar","car"]), "")
        self.assertEqual(LongestCommonPrefix(["aa","aa"]), "aa")
        self.assertEqual(LongestCommonPrefix(["aa","bb"]), "")


#={===========================================================================
if __name__ == '__main__':
    unittest.main()
