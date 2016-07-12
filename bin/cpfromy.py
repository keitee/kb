#!/usr/bin/env python
import sys
import subprocess

if __name__ == "__main__":
    # execute only if run as a script

    if len(sys.argv) > 2:
        # print error and exit
        sys.stdout.write("usuage: cpfromy {darwin_0019fbf2d46b_fe6f83272099db83e29c10fc8cbb83aa} filename\n")
        sys.exit(1)

    fullpath = "si_logs@theyard:/home/si_logs/translation/" + sys.argv[1] + "/LOGlastrun_realtime"

    sys.stdout.write("%d: copy %s from theyard to current dicrectory...\n" % \
            (len(sys.argv), fullpath))

    subprocess.call(["scp", "-r", fullpath, "."])

