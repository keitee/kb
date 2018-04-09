#!/usr/bin/env python
import sys
import subprocess
import re

if __name__ == "__main__":
    # execute only if run as a script

    if len(sys.argv) > 2:
        # print error and exit
        sys.stdout.write("usuage: cpfromy */darwin_0019fbf2d46b_fe6f83272099db83e29c10fc8cbb83aa/* \n")
        sys.exit(1)

    logfile = "LOGlastrun_realtime"

    # to support:
    # http://theyard.cisco.com/si_logs/translation/darwin_783e53fe4a60_81b952e11b7e50199c8d07a4649310c7/index.html
    # darwin_783e53ffaa6d_9276a4c871ae42a6a41f729def322c3f.tgz

    # why do not cover both?
    # match = re.match('(darwin_[0-9A-Za-z]{12}_[0-9A-Za-z]{32})', sys.argv[1])
    match = re.match('.*(darwin_[0-9A-Za-z]{12}_[0-9A-Za-z]{32})', sys.argv[1])

    filename = match.group(1)

    fullpath = "si_logs@theyard:/home/si_logs/translation/" + filename + "/" + logfile

    # scp si_logs@theyard:/home/si_logs/translation/darwin_783e53fe4a60_81b952e11b7e50199c8d07a4649310c7/LOGlastrun_realtime from theyard to current dicrectory...
    sys.stdout.write("scp %s from theyard to current dicrectory...\n" % fullpath)

    subprocess.call(["scp", "-r", fullpath, "."])

    # add last 4 chars to the log filename
    splited = filename.split("_")
    newlogfile = logfile + "_" + splited[2][-4:]
    sys.stdout.write("mv %s %s\n" % (logfile, newlogfile))
    subprocess.call(["mv", logfile, newlogfile])

    text = "1i" + filename
    subprocess.call(["sed", "-i", text, newlogfile])
