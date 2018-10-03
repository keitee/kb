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
        sys.stdout.write("builds asan/static/no uaf using target tools ...\n")
        subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "-fsanitize=address", "uaf_mod.c", "-o", "uaf_target_asan"])
        subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "-static-libasan", "-fsanitize=address", "uaf_mod.c", "-o", "uaf_target_static_asan"])
        subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "uaf_mod.c", "-o", "uaf_target_no_asan"])
        sys.stdout.write("done\n")

    if args.host:
        sys.stdout.write("builds asan/static/no uaf using host tools ...\n")
        subprocess.call(["gcc", "-g", "-fsanitize=address", "uaf_mod.c", "-o", "uaf_host_asan"])
        subprocess.call(["gcc", "-g", "-static-libasan", "-fsanitize=address", "uaf_mod.c", "-o", "uaf_host_static_asan"])
        subprocess.call(["gcc", "-g", "uaf_mod.c", "-o", "uaf_host_no_asan"])
        sys.stdout.write("done\n")

    # for clang which use static by default
    if args.clang:
        sys.stdout.write("builds asan/static/no uaf using clang tools ...\n")
        subprocess.call(["/home/kyoupark/asan/llvm_cmake_build/bin/clang", "-g", "-O1", "-fno-omit-frame-pointer", "-fsanitize=address", "uaf_mod.c", "-o", "uaf_clang_asan"])
        subprocess.call(["/home/kyoupark/asan/llvm_cmake_build/bin/clang", "-g", "-O1", "uaf_mod.c", "-o", "uaf_clang_no_asan"])
        sys.stdout.write("done\n")
