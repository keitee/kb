#!/usr/bin/env python
import sys
import subprocess
import argparse

# execute only if run as a script
if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("--host32", help="build for host 32", action="store_true")
    parser.add_argument("--host32m", help="build for host 32m", action="store_true")
    parser.add_argument("--host64", help="build for host 64", action="store_true")
    parser.add_argument("--host64_recover", help="build for host 64 with recover", action="store_true")
    parser.add_argument("--target", help="build for target", action="store_true")
    parser.add_argument("--clang", help="build for clang", action="store_true")
    args = parser.parse_args()

    # 32 bits output may not work on 64 bit host
    if args.target:
        sys.stdout.write("builds asan/static/no gbo using target tools ...\n")
        subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "-fsanitize=address", "gbo.c", "-o", "gbo_target_asan_32"])
        subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "-static-libasan", "-fsanitize=address", "gbo.c", "-o", "gbo_target_static_asan_32"])
        subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "gbo.c", "-o", "gbo_target_no_asan_32"])
        sys.stdout.write("done\n")

    if args.host32:
        sys.stdout.write("builds asan/static/no gbo using 32 host tools ...\n")
        subprocess.call(["gcc", "-g", "-fsanitize=address", "gbo.c", "-o", "gbo_host_asan_32"])
        subprocess.call(["gcc", "-g", "-static-libasan", "-fsanitize=address", "gbo.c", "-o", "gbo_host_static_asan_32"])
        subprocess.call(["gcc", "-g", "gbo.c", "-o", "gbo_host_no_asan_32"])
        sys.stdout.write("done\n")

    # do not work
    # if args.host32m:
    #     sys.stdout.write("builds asan/static/no gbo using m32 host tools ...\n")
    #     subprocess.call(["gcc", "-m32", "-g", "-fsanitize=address", "gbo.c", "-o", "gbo_host_asan_m32"])
    #     subprocess.call(["gcc", "-m32", "-g", "-static-libasan", "-fsanitize=address", "gbo.c", "-o", "gbo_host_static_asan_m32"])
    #     subprocess.call(["gcc", "-m32", "-g", "gbo.c", "-o", "gbo_host_no_asan_m32"])
    #     sys.stdout.write("done\n")

    if args.host64:
        sys.stdout.write("builds asan/static/no gbo using 64 host tools ...\n")
        subprocess.call(["gcc", "-g", "-fsanitize=address", "gbo.c", "-o", "gbo_host_asan_64"])
        subprocess.call(["gcc", "-g", "-static-libasan", "-fsanitize=address", "gbo.c", "-o", "gbo_host_static_asan_64"])
        subprocess.call(["gcc", "-g", "gbo.c", "-o", "gbo_host_no_asan_64"])
        sys.stdout.write("done\n")

    if args.host64_recover:
        sys.stdout.write("builds asan/static/no gbo using 64 host tools ...\n")
        subprocess.call(["gcc", "-g", "-fsanitize-recover=address", "gbo_recover.c", "-o", "gbo_host_asan_64_recover"])
        subprocess.call(["gcc", "-g", "-static-libasan", "-fsanitize-recover=address", "gbo_recover.c", "-o", "gbo_host_static_asan_64_recover"])
        subprocess.call(["gcc", "-g", "gbo_recover.c", "-o", "gbo_host_no_asan_64_recover"])
        sys.stdout.write("done\n")

    # for clang which use static by default
    if args.clang:
        sys.stdout.write("builds asan/static/no gbo using clang tools ...\n")
        subprocess.call(["/home/kyoupark/asan/llvm_cmake_build/bin/clang", "-g", "-O1", "-fno-omit-frame-pointer", "-fsanitize=address", "gbo.c", "-o", "gbo_clang_asan"])
        subprocess.call(["/home/kyoupark/asan/llvm_cmake_build/bin/clang", "-g", "-O1", "gbo.c", "-o", "gbo_clang_no_asan"])
        sys.stdout.write("done\n")
