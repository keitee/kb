# This recipe inherits the confbuild class.
# If we run now bitbake third it will execute the configure and build tasks for third.

DESCRIPTION = "I am the third recipe"
PR = "r1"
inherit confbuild
