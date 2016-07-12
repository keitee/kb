#!/usr/bin/env python
import sys
import subprocess

if __name__ == "__main__":
    # execute only if run as a script

    if len(sys.argv) > 2:
        # print error and exit
        sys.stdout.write("usuage: cptoy {filename}\n")
        sys.exit(1)

    sys.stdout.write("%d: copy %s to theyard:~/si_logs/kit.park...\n" % \
            (len(sys.argv), sys.argv[1]))

    subprocess.call(["scp", sys.argv[1], "si_logs@theyard:/home/si_logs/kit.park"])

