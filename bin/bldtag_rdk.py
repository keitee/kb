#!/usr/bin/env python
import sys
import subprocess

if __name__ == "__main__":
    # execute only if run as a script

    # cleanup files
    subprocess.call(["rm", "-f", "G* tags flist.out"])

    # runs on /home/keitee/rdk/appinfrastructure/RDK
    # dirs to include
    dirs = [
        'meta-rdk-sky',
        'as-system-service',
        'generic/devicesettings',
        'generic/iarmmgrs',
        'build-arrisxi6-morty/tmp/work/arrisxi6_morty-rdk-linux-gnueabi/devicesettings'
        ]

    listfile = 'flist.out'
    logfile = open(listfile, 'w+')

    # dirs to exclude
    # find {to be replaced} -type d \( -path '*/build' -o -path '*/mock' \) -prune -o -print
    # find BluetoothRcu/daemon -type d \( -path '*/build' -o -path '*/mock' \) -prune -o -print
    # subprocess.call(["find", ".", "-type d \( -path '*/build' -o -path '*/mock' \)", "-prune", "-o", "-print"])
    # note that no "-o" before -print
    command = ["find", "-L", ".", "-type", "d", 
        "(", 
        # "-path",  "*/build", "-o", 
        "-path",  "*/build", "-o",
        "-path",  "*/*.cache", "-o",
        "-path",  "*/.git",
        # note on this end entry which is differnt from the above
        # "-path", "*/test",
        ")", "-prune", "-o", "-type", "f", "-print"]

    for e in dirs:
      # replace arg[2] for starting dir
      command[2] = e
      print command
      subprocess.call(command, stdout=logfile)

    logfile.close()

    sys.stdout.write("building ctags for %s ...\n" % listfile)
    subprocess.call(["ctags", "-L", listfile])

    sys.stdout.write("building gtags for %s ...\n" % listfile)
    subprocess.call(["gtags", "--skip-unreadable", "-f", listfile])

    sys.stdout.write("done\n")
