#!/usr/bin/env python

import sys
import subprocess

if __name__ == "__main__":
    # execute only if run as a script

    if len(sys.argv) < 3:
        # print error and exit
        # e.g., file is LOGlastrun_realtime_1c75
        sys.stdout.write("mgrep {pattern} {file} ?{outfile} \n")
        sys.exit(1)

    # when pattern is $ cmd "SS|DD", pattern var gets SS|DD and subproces call
    # works okay.
    pattern = sys.argv[1]
    log_filename = sys.argv[2]

    # use default out_filename when no outfile is provided.
    if len(sys.argv) < 4:
      out_filename = log_filename.split('_')[-1] + '.nds'
    else:
      out_filename = sys.argv[3]

    sys.stdout.write('egrep -an "%s" %s > %s\n' % (pattern, log_filename, out_filename))

    o = open(out_filename, 'w+')
    o.write('%s\n' % ('='*60))
    o.write('egrep -an "%s" %s > %s\n' % (pattern, log_filename, out_filename))
    o.write('%s\n' % ('='*60))

    # os.SEEK_END or 2 (seek relative to the file's end).
    o.seek(0, 2)

    # egrep -an pattern logfile > out_filename
    subprocess.call(["egrep", "-an", pattern, log_filename], stdout=o)

    o.close()
