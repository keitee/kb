#!/usr/bin/env python
import sys
import subprocess

if __name__ == "__main__":
    # execute only if run as a script

    # cleanup files
    subprocess.call(["rm", "-f", "G* tags flist.out"])

    # dirs to include
    dirs = [
        '/home/keitee/mw/ethan-as-source/Components/AudioStreamer/include',
        '/home/keitee/mw/ethan-as-source/Components/AudioStreamer/webpage',
        '/home/keitee/mw/ethan-as-source/Components/AudioStreamer/app',
        '/home/keitee/mw/ethan-as-source/Components/AudioStreamer/tools',
        '/home/keitee/mw/ethan-as-source/Components/AudioStreamer/unitTest',
        '/home/keitee/mw/ethan-as-source/Components/AudioStreamer/config',
        '/home/keitee/mw/ethan-as-source/Components/AudioStreamer/build',
        '/home/keitee/mw/ethan-as-source/Components/AudioStreamer/drivers',
        '/home/keitee/mw/ethan-as-source/Components/AudioStreamer/wifiApp',
        '/home/keitee/mw/ethan-as-source/Components/AudioStreamer/source',
        '/home/keitee/mw/ethan-as-source/Components/AudioStreamer/AirplaySdk2'
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
        "-path", "*/VQE_SRC",  "-o",
        "-path", "*/AVCU",  "-o",
        "-path", "*/Interface",  "-o",
        "-path", "*/doc",  "-o",
        "-path", "*/docs",  "-o",
        "-path", "*/openssl",  "-o",
        "-path", "*/FREETYPE2",  "-o",
        "-path", "*/iscdhcp",  "-o",
        "-path", "*/unit_test",  "-o",
        "-path", "*/cunitTest",  "-o",
        "-path", "*/test",  "-o",
        "-path", "*/test2",
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
