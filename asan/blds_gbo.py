#!/usr/bin/env python
import sys
import subprocess
import argparse

# execute only if run as a script
if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("--host", help="build for host", action="store_true")
    parser.add_argument("--target", help="build for target", action="store_true")
    parser.add_argument("--clang", help="build for clang", action="store_true")
    args = parser.parse_args()

    # 32 bits output may not work on 64 bit host
    if args.target:
        sys.stdout.write("builds asan/static/no gbo using target tools ...\n")
        subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "-fsanitize=address", "gbo.c", "-o", "gbo_target_asan"])
        subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "-static-libasan", "-fsanitize=address", "gbo.c", "-o", "gbo_target_static_asan"])
        subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "gbo.c", "-o", "gbo_target_no_asan"])
        sys.stdout.write("done\n")

    if args.host:
        sys.stdout.write("builds asan/static/no gbo using host tools ...\n")
        subprocess.call(["gcc", "-g", "-fsanitize=address", "gbo.c", "-o", "gbo_host_asan_32"])
        subprocess.call(["gcc", "-g", "-static-libasan", "-fsanitize=address", "gbo.c", "-o", "gbo_host_static_asan_32"])
        subprocess.call(["gcc", "-g", "gbo.c", "-o", "gbo_host_no_asan_32"])
        sys.stdout.write("done\n")

    # for clang which use static by default
    if args.clang:
        sys.stdout.write("builds asan/static/no gbo using clang tools ...\n")
        subprocess.call(["/home/kyoupark/asan/llvm_cmake_build/bin/clang", "-g", "-O1", "-fno-omit-frame-pointer", "-fsanitize=address", "gbo.c", "-o", "gbo_clang_asan"])
        subprocess.call(["/home/kyoupark/asan/llvm_cmake_build/bin/clang", "-g", "-O1", "gbo.c", "-o", "gbo_clang_no_asan"])
        sys.stdout.write("done\n")
