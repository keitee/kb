#!/usr/bin/env python
import sys
import subprocess

if __name__ == "__main__":
    # execute only if run as a script

    # cleanup files
    subprocess.call(["rm", "-f", "G* tags flist.out"])

    # where to run /home/keitee/ip/SkyBluetoothRcu
    # dirs to include
    dirs = [
        'app/src/main/cpp/blercu',
        'app/src/main/java',
        'app/src/main/cpp/configsettings'
        ]

    listfile = 'flist.out'
    logfile = open(listfile, 'w+')

    # dirs to exclude
    # find {to be replaced} -type d \( -path '*/build' -o -path '*/mock' \) -prune -o -print
    # subprocess.call(["find", ".", "-type d \( -path '*/build' -o -path '*/mock' \)", "-prune", "-o", "-print"])
    command = ["find", ".", "-type", "d", 
        "(", 
        "-path",  "*/build", "-o", 
        "-path", "*/mock",  "-o",
    #    "-path", "*/lib",  "-o",
        "-path", "*/tools",  "-o",
        "-path", "*/Interface",  "-o",
        "-path", "*/doc",  "-o",
        "-path", "*/docs",  "-o",
        "-path", "*/unit_test",  "-o",
        "-path", "*/cunitTest",  "-o",
        "-path", "*/test",  "-o",
        "-path", "*/android",
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
