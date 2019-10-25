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

