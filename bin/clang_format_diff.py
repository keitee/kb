#!/usr/bin/env python

# #!/bin/bash
# styles=( LLVM Google Chromium Mozilla WebKit )
# for s in LLVM Google Chromium Mozilla WebKit
# do
#   echo $s...
#   clang-format -style=$s blercucontroller.cpp > blercucontroller_$s.cpp
# done

import sys
import subprocess

if __name__ == "__main__":
    # execute only if run as a script

    # py-list. formats to try
    styles = [
        "LLVM", "Google", "Chromium", "Mozilla", "WebKit"
        ]

    # dirs to exclude
    # find {to be replaced} -type d \( -path '*/build' -o -path '*/mock' \) -prune -o -print
    # subprocess.call(["find", ".", "-type d \( -path '*/build' -o -path '*/mock' \)", "-prune", "-o", "-print"])
    # clang-format -style={style} ChLcpIterativeAPGD.h > ChLcpIterativeAPGD.$style.h

    for e in styles:
      sys.stdout.write("clang-format for %s ...\n" % e)
      fname = sys.argv[1] + "_" + e
      fout = open(fname, 'w+')
      style = "-style=" + e 
      command = ["clang-format", style, sys.argv[1]]
      subprocess.call(command, stdout=fout)
      fout.close()

