#!/usr/bin/env python
import sys
import subprocess
import argparse

# execute only if run as a script
if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("--target_32_normal", help="build for 32, 630", action="store_true")
    parser.add_argument("--target_32_recover", help="build for 32, 630, recover", action="store_true")
    parser.add_argument("--host_32_normal", help="build for 32, 630", action="store_true")
    parser.add_argument("--host_32_recover", help="build for 32, 630, recover", action="store_true")
    parser.add_argument("--host_32_clang", help="build for clang", action="store_true")
    args = parser.parse_args()

    # 32 bits output may not work on 64 bit host
    # if args.target:
    #     sys.stdout.write("builds asan/static/no gbo using target tools ...\n")
    #     subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "-fsanitize=address", "gbo.c", "-o", "gbo_target_asan_32"])
    #     subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "-static-libasan", "-fsanitize=address", "gbo.c", "-o", "gbo_target_static_asan_32"])
    #     subprocess.call(["/home/kyoupark/asan/gcc/i686-nptl-linux-gnu-gcc/bin/i686-nptl-linux-gnu-gcc", "-g", "gbo.c", "-o", "gbo_target_no_asan_32"])
    #     sys.stdout.write("done\n")

    # 32 bit target, toolchain to match this target and modified asan which do not die. gcc630
    # -fsanitize=address
    #
    # both works
    # ./gbo_targe_host_asan_32
    # LD_LIBRARY_PATH=/home/kyoupark/x-tools/i686-nptl-linux-gnu/i686-nptl-linux-gnu/lib ./gbo_target_host_asan_32

    if args.target_32_normal:
        sys.stdout.write("builds 32, gcc 630 ...\n")
        subprocess.call(["/home/kyoupark/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc", "-g", "-fsanitize=address", "gbo_recover.c", "-o", "gbo_32_630_asan"])
        # subprocess.call(["/home/kyoupark/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc", "-g", "-static-libasan", "-fsanitize=address", "gbo.c", "-o", "gbo_target_host_static_asan_32"])
        subprocess.call(["/home/kyoupark/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc", "-g", "gbo_recover.c", "-o", "gbo_32_630_no_asan"])
        sys.stdout.write("done\n")

    # 32 bit target, toolchain to match this target and modified asan which do not die. gcc630
    # -fsanitize-recover=address
    if args.target_32_recover:
        sys.stdout.write("builds 32, gcc 630 with recover ...\n")
        subprocess.call(["/home/kyoupark/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc", "-g", "-fsanitize=address", "-fsanitize-recover=address", "gbo_recover.c", "-o", "gbo_32_630_asan_recover"])
        # subprocess.call(["/home/kyoupark/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc", "-g", "-static-libasan", "-fsanitize=address", "gbo.c", "-o", "gbo_target_host_static_asan_32"])
        # subprocess.call(["/home/kyoupark/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc", "-g", "gbo_recover.c", "-o", "gbo_32_630_no_asan"])
        sys.stdout.write("done\n")

    # 32 bit debian host toolchani
    if args.host_32_normal:
        sys.stdout.write("builds 32, host gcc 630 ...\n")
        subprocess.call(["gcc", "-g", "-fsanitize=address", "gbo_recover.c", "-o", "gbo_host_32_630_asan"])
        subprocess.call(["gcc", "-g", "gbo_recover.c", "-o", "gbo_host_32_630_no_asan"])
        sys.stdout.write("done\n")

    # 32 bit debian host toolchani
    if args.host_32_recover:
        sys.stdout.write("builds 32, host gcc 630 ...\n")
        subprocess.call(["gcc", "-g", "-fsanitize=address", "-fsanitize-recover=address", "gbo_recover.c", "-o", "gbo_host_32_630_asan_recover"])
        # subprocess.call(["gcc", "-g", "gbo_recover.c", "-o", "gbo_host_32_630_no_asan"])
        sys.stdout.write("done\n")


    # do not work
    # if args.host32m:
    #     sys.stdout.write("builds asan/static/no gbo using m32 host tools ...\n")
    #     subprocess.call(["gcc", "-m32", "-g", "-fsanitize=address", "gbo.c", "-o", "gbo_host_asan_m32"])
    #     subprocess.call(["gcc", "-m32", "-g", "-static-libasan", "-fsanitize=address", "gbo.c", "-o", "gbo_host_static_asan_m32"])
    #     subprocess.call(["gcc", "-m32", "-g", "gbo.c", "-o", "gbo_host_no_asan_m32"])
    #     sys.stdout.write("done\n")

    # llvm on debian office dist
    # for clang which use static by default
    if args.host_32_clang:
        sys.stdout.write("builds asan/static/no gbo using clang tools ...\n")
        # subprocess.call(["/home/kyoupark/asan/llvm_cmake_build/bin/clang", "-g", "-O1", "-fno-omit-frame-pointer", "-fsanitize=address", "gbo.c", "-o", "gbo_clang_asan"])
        # subprocess.call(["/home/kyoupark/asan/llvm_cmake_build/bin/clang", "-g", "-O1", "gbo.c", "-o", "gbo_clang_no_asan"])
        subprocess.call(["/home/kyoupark/asan/llvm/cmake_build/bin/clang", "-g", "-fno-omit-frame-pointer", "-fsanitize=address", "gbo_recover.c", "-o", "gbo_host_32_clang_asan"])
        subprocess.call(["/home/kyoupark/asan/llvm/cmake_build/bin/clang", "-g", "-fno-omit-frame-pointer", "-fsanitize-recover=address", "gbo_recover.c", "-o", "gbo_host_32_clang_asan_recover"])
        subprocess.call(["/home/kyoupark/asan/llvm/cmake_build/bin/clang", "-g", "-fno-omit-frame-pointer", "-fsanitize=address", "-fsanitize-blacklist=blacklist.txt", "gbo_recover.c", "-o", "gbo_host_32_clang_asan_black"])
        sys.stdout.write("done\n")
