#!/usr/bin/env python
import sys
import subprocess

if __name__ == "__main__":
    # execute only if run as a script

    if len(sys.argv) > 2:
        # print error and exit
        sys.stdout.write("usuage: cpfromy {darwin_0019fbf2d46b_fe6f83272099db83e29c10fc8cbb83aa.tgz} \n")
        sys.exit(1)

    logfile = "LOGlastrun_realtime"
    filename = sys.argv[1].split(".")[0]
    fullpath = "si_logs@theyard:/home/si_logs/translation/" + filename + "/" + logfile

    sys.stdout.write("scp %s from theyard to current dicrectory...\n" % fullpath)

    subprocess.call(["scp", "-r", fullpath, "."])

    # add last 4 chars to the log filename
    splited = filename.split("_")
    newlogfile = logfile + "_" + splited[2][-4:]
    sys.stdout.write("mv %s %s\n" % (logfile, newlogfile))
    subprocess.call(["mv", logfile, newlogfile])
