import unittest

# $ python coll_test.py --verbose
#
# to run single test
# $ python -m unittest coll_test.TestDictCreation.test_create_by_assign
# 
# to run all of a class
# $ python -m unittest coll_test.TestDictCreation

#={===========================================================================
# py-attribute

class TestAttribute(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN] ", self._testMethodName


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
        print "===================="
        print "[RUN] ", self._testMethodName

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


class TestNumeric(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN] ", self._testMethodName

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


class TestString(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN] ", self._testMethodName

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
# py-print

class TestPrint(unittest.TestCase):

    def setUp(self):
        print "===================="
        print "[RUN] ", self._testMethodName

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

        print 'item in the list: %s' % item
        print item
        print(item)

        pay = 10000
        print('%.2f' % pay)
        print('{0:.2f}'.format(pay))

        # numbers are optional
        print('{:.2f}'.format(pay))

        print '%s, eggs, and %s' % ('spam', 'SPAM!') 

        print '{0}, eggs, and {1}'.format('spam', 'SPAM!') 

        # Numbers optional (2.7+, 3.1+)
        print '{}, eggs, and {}'.format('spam', 'SPAM!') 

        print '{0:o}, {1:x}, {2:b}'.format(64, 64, 64)

        print '%s -- %s -- %s' % (42, 3.14159, [1, 2, 3]) 


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
if __name__ == '__main__':
    unittest.main()
