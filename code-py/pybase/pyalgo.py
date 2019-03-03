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
# py-leetcode-012, CountAndSay
# from leetcode online comments

class Solution_CountAndSay:
    def countAndSay(self, n):
        """
        :type n: int
        :rtype: str
        """
        if n == 1:
            return "1"
        return self.count(self.countAndSay(n - 1))

    def count(self , s):
        c = s[0]
        count = 1
        res = ""
        for char in s[1 : ]:
            if char == c:
                count += 1
            else:
                res  = res + str(count) + c
                c = char
                count = 1

        res = res + str(count) + c
        return res


#={===========================================================================
# py-leetcode-013, 53. Maximum Subarray
# from leetcode online comments

#   0  1   2  3  4   5  6   7  8
# [-2, 1, -3, 4, -1, 2, 1, -5, 4]
#
# i: 2, nums[2] : -3 += nums[1] : 1
# nums[2] : -2
# i: 4, nums[4] : -1 += nums[3] : 4
# nums[4] : 3
# i: 5, nums[5] : 2 += nums[4] : 3
# nums[5] : 5
# i: 6, nums[6] : 1 += nums[5] : 5
# nums[6] : 6
# i: 7, nums[7] : -5 += nums[6] : 6
# nums[7] : 1
# i: 8, nums[8] : 4 += nums[7] : 1
# nums[8] : 5
# nums: [-2, 1, -2, 4, 3, 5, 6, 1, 5]

class Solution_MaxSubArray:
    def answer(self, nums):
        for i in range(1, len(nums)):
            if nums[i-1] > 0:
                print('i: %s, nums[%s] : %s += nums[%s] : %s' % 
                        (i, i, nums[i], i-1, nums[i-1]))
                nums[i] += nums[i-1]
                print('nums[%s] : %s' % (i, nums[i]))
        print('nums: %s' % nums)        
        return max(nums)


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

    def test_algo_count_and_say(self):
        co = Solution_CountAndSay()
        print(co.countAndSay(6))
        print(co.countAndSay(7))

    def test_algo_max_sub_array(self):
        co = Solution_MaxSubArray()
        print(co.answer([-2,1,-3,4,-1,2,1,-5,4]))


#={===========================================================================
if __name__ == '__main__':
    unittest.main()
