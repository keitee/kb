#!/usr/bin/env python
import sys
import subprocess

if __name__ == "__main__":
    # execute only if run as a script

    # cleanup files
    subprocess.call(["rm", "-f", "G* tags flist.out"])

    # runs on /ethan-as-source for both as and ai
    # dirs to include
    dirs = [
        'app/src/main/java/com/sky/',
        'clientlib/src/main/java/',
        'example/app/src/',
        'app/src/main/cpp/'
        ]

    listfile = 'flist.out'
    logfile = open(listfile, 'w+')

    # dirs to exclude
    # find {to be replaced} -type d \( -path '*/build' -o -path '*/mock' \) -prune -o -print
    # subprocess.call(["find", ".", "-type d \( -path '*/build' -o -path '*/mock' \)", "-prune", "-o", "-print"])
    command = ["find", ".", "-type", "d", 
        "(", 
        # "-path",  "*/build", "-o", 
        "-path",  "*/build",
        # note on this end entry which is differnt from the above
        # "-path", "*/test",
        ")", "-prune", "-o", "-type", "f", "-print"]

    for e in dirs:
      command[1] = e
      subprocess.call(command, stdout=logfile)

    logfile.close()

    sys.stdout.write("building ctags for %s ...\n" % listfile)
    subprocess.call(["ctags", "-L", listfile])

    sys.stdout.write("building gtags for %s ...\n" % listfile)
    subprocess.call(["gtags", "--skip-unreadable", "-f", listfile])

    sys.stdout.write("done\n")
