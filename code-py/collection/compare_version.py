import unittest


# returns 1 if greater is bigger than lesser
#        -1 is lesser  is bigger than greater
#         0 if they are equal
def compareVersions(self, greater, lesser):
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

class TestStringMethods(unittest.TestCase):

    def test_compare_version(self):
        if compareVersions(version1, version2) is -1:
            print "the second is bigger than the first"

if __name__ == '__main__':
    unittest.main()
