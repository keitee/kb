import unittest


# returns 1 if greater is bigger than lesser
#        -1 is lesser  is bigger than greater
#         0 if they are equal

def compareVersions(greater, lesser):

    # covert it to int list
    greater_array = [int(i) for i in greater.split('.')]
    lesser_array = [int(i) for i in lesser.split('.')]

    len_lesser = len(lesser_array)

    for i in range(len(greater_array)):
        try:
            if i >= len_lesser or greater_array[i] > lesser_array[i]:
                return 1
            elif greater_array[i] < lesser_array[i]:
                return -1
        except IndexError:
            # One of the versions strings is too short, assume they are the same
            print 'Warning unmatched version string lengths', greater, lesser                

    # if we get here they should be the same
    return 0


"""
version scheme has units.
    XXX.XXX.XXX
    XXX             that's 100's unit when used the first
    XXX             that's 1's unit when used the second

"""

class TestMethods(unittest.TestCase):

    def test_compare_version_1(self):

        version1 = '040.86.00'
        version2 = '040.83.99'

        result = compareVersions(version1, version2)
        self.assertEqual(result, 1)

    # the first is bigger
    def test_compare_version_2(self):

        version1 = '040.86.00'
        version2 = '040'

        result = compareVersions(version1, version2)
        self.assertEqual(result, 1)

    # regarded as the same
    def test_compare_version_3(self):

        version1 = '040'
        version2 = '040.86.00'

        result = compareVersions(version1, version2)
        self.assertEqual(result, 0)

if __name__ == '__main__':
    unittest.main()
