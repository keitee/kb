#!/usr/bin/env python
import sys
import subprocess

if __name__ == "__main__":
    # execute only if run as a script

    sys.stdout.write("buils asan uaf using own tools ...\n")
    subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "-fsanitize=address", "uaf_mod.c", "-o", "own_asan_uaf"])

    sys.stdout.write("buils static asan uaf using own tools ...\n")
    subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "-static-libasan", "-fsanitize=address", "uaf_mod.c", "-o", "own_static_asan_uaf"])

    sys.stdout.write("buils no asan uaf using own tools ...\n")
    subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "uaf_mod.c", "-o", "own_no_asan_uaf"])

    sys.stdout.write("done\n")

