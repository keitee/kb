# CHAPTER 29 Class Coding Details
# Documentation Strings Revisited

# >>> import pyclassdoc
# >>> pyclassdoc.__doc__
# 'I am: docstr.__doc__'
# >>> pyclassdoc.func.__doc__
# 'I am: docstr.func.__doc__'
# >>> pyclassdoc.spam.__doc__
# 'I am: spam.__doc__ or docstr.spam.__doc__ or self.__doc__'
# >>> pyclassdoc.spam.method.__doc__
# 'I am: spam.method.__doc__ or self.method.__doc__'
# "I am: docstr.__doc__"

# >>> help(pyclassdoc)
#
# Help on module pyclassdoc:
# 
# NAME
#     pyclassdoc - I am: docstr.__doc__
# 
# FILE
#     /home/kyoupark/git/kb/code-py/pyclass/pyclassdoc.py
# 
# CLASSES
#     spam
# 
#     class spam
#      |  I am: spam.__doc__ or docstr.spam.__doc__ or self.__doc__
#      |
#      |  Methods defined here:
#      |
#      |  method(self)
#      |      I am: spam.method.__doc__ or self.method.__doc__
# 
# FUNCTIONS
#     func(args)
#         I am: docstr.func.__doc__

def func(args):
    "I am: docstr.func.__doc__"
    pass

class spam:

    "I am: spam.__doc__ or docstr.spam.__doc__ or self.__doc__"

    def method(self):
        "I am: spam.method.__doc__ or self.method.__doc__"
        print(self.__doc__)
        print(self.method.__doc__)

