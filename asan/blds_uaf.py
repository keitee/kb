#!/usr/bin/env python
import sys
import subprocess
import argparse

# execute only if run as a script
if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("--host", help="build for host", action="store_true")
    parser.add_argument("--target_494", help="build for target", action="store_true")
    parser.add_argument("--target_494_recover", help="build for target", action="store_true")
    parser.add_argument("--target_630", help="build for target", action="store_true")
    parser.add_argument("--target_630_recover", help="build for target", action="store_true")
    parser.add_argument("--clang", help="build for clang", action="store_true")
    args = parser.parse_args()

    # 32 bits output may not work on 64 bit host
    # 32 bit, gcc494
    if args.target_494:
        sys.stdout.write("builds 32, gcc 494, ...\n")
        subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "-fsanitize=address", "uaf_mod.c", "-o", "uaf_32_494_asan"])
        # subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "-static-libasan", "-fsanitize=address", "uaf_mod.c", "-o", "uaf_target_static_asan"])
        subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "uaf_mod.c", "-o", "uaf_32_494_no_asan"])
        sys.stdout.write("done\n")

    if args.target_494_recover:
        sys.stdout.write("builds 32, gcc 494, ...\n")
        subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "-fsanitize-recover=address", "uaf_mod.c", "-o", "uaf_32_494_asan"])
        # subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "-static-libasan", "-fsanitize=address", "uaf_mod.c", "-o", "uaf_target_static_asan"])
        # subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "uaf_mod.c", "-o", "uaf_32_494_no_asan"])
        sys.stdout.write("done\n")

    if args.target_630:
        sys.stdout.write("builds 32, gcc 630 ...\n")
        subprocess.call(["/home/kyoupark/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc", "-g", "-fsanitize=address", "uaf_mod.c", "-o", "uaf_32_630_asan"])
        # subprocess.call(["/home/kyoupark/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc", "-g", "-static-libasan", "-fsanitize=address", "gbo.c", "-o", "gbo_target_host_static_asan_32"])
        subprocess.call(["/home/kyoupark/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc", "-g", "uaf_mod.c", "-o", "uaf_32_630_no_asan"])
        sys.stdout.write("done\n")

    if args.target_630_recover:
        sys.stdout.write("builds 32, gcc 630 ...\n")
        subprocess.call(["/home/kyoupark/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc", "-g", "-fsanitize-recover=address", "uaf_mod.c", "-o", "uaf_32_630_asan_recover"])
        # subprocess.call(["/home/kyoupark/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc", "-g", "-static-libasan", "-fsanitize=address", "gbo.c", "-o", "gbo_target_host_static_asan_32"])
        # subprocess.call(["/home/kyoupark/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc", "-g", "uaf_mod.c", "-o", "uaf_32_630_no_asan"])
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
