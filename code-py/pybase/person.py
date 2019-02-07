# CHAPTER 28 A More Realistic Example

class Person:

    # *py-default-argument*
    # As with C++, any arguments in a function header after the first default
    # must all have defaults, too:

    def __init__(self, name, job=None, pay=0):
        self.name = name
        self.job = job
        self.pay = pay

    def lastName(self):
        return self.name.split()[-1]

    def giveRaise(self, percent):
        self.pay = int(self.pay * (1 + percent))

    def __repr__(self):
        return '[Person: %s, %s, %s]' % (self.name, self.job, self.pay)

class Manager(Person):

        # because Manager had no __init__ constructor, it inherits that
        # in Person.

    def giveRaise(self, percent, bonus=.10):
        # Bad: cut and paste
        # self.pay = int(self.pay * (1 + percent + bonus)) 

        # *py-class-call-base-version*
        # Good: augment original
        Person.giveRaise(self, percent + bonus)


# kyoupark@kit-debian64:~/git/kb/code-py/pyclass$ python person.py 
# ('Bob Smith', 0)
# ('Sue Jones', 100000)

bob = Person('Bob Smith')
sue = Person('Sue Jones', job='dev', pay=100000)

print(bob.name, bob.pay)            # Fetch attached attributes
print(sue.name, sue.pay)            # sue's and bob's attrs differ

print(bob.name.split()[-1])         # Extract object's last name
sue.pay *= 1.10                     # Give this object a raise

#print('%.2f' % sue.pay)
#print('{:.2f}'.format(sue.pay))
print '{:.2f}'.format(sue.pay)

