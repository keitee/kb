# 6.2. Use myclass with the second recipe
#
# DESCRIPTION and PR are as usual.
#
# >> inherit mybuild
#
# The mybuild class becomes inherited and so myclass_do_build() becomes the
# "default build task."
#
# The (pure python) function pyfunc takes some argument and runs the python dir
# function on this argumentm and prints the result.
#
# The (bitbake python) mypatch function is added and registered as a task that
# needs to be executed before the build function.
#
# mypatch calls pyfunc and passes the global bitbake variable d.
#
# d (datastore) is defined by bitbake and is always available.
#
# >> addtask mypatch before do_build
#
# The mypatch function is registered as a task that needs to be executed before
# the build function.

DESCRIPTION = "I am the second recipe"

PR = "r1"

inherit mybuild

def pyfunc(o):
    print(dir(o))

python do_mypatch () {
  bb.note ("runnin mypatch")
  pyfunc(d)
}

addtask mypatch before do_build

