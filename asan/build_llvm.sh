#!/bin/sh

# https://github.com/google/sanitizers/wiki/AddressSanitizerHowToBuild

# cd somewhere
# Get llvm, clang and compiler-rt
svn co http://llvm.org/svn/llvm-project/llvm/trunk llvm
cd llvm
R=$(svn info | grep Revision: | awk '{print $2}')
(cd tools && svn co -r $R http://llvm.org/svn/llvm-project/cfe/trunk clang)
(cd projects && svn co -r $R http://llvm.org/svn/llvm-project/compiler-rt/trunk compiler-rt)

# To switch your checkout to read-write mode:
# CNAME=your_name_here
CNAME=kit
cd llvm
svn switch --relocate http://llvm.org/svn/llvm-project/llvm/trunk https://$CNAME@llvm.org/svn/llvm-project/llvm/trunk
(cd tools && svn switch --relocate http://llvm.org/svn/llvm-project/cfe/trunk https://$CNAME@llvm.org/svn/llvm-project/cfe/trunk clang)
(cd projects && svn switch --relocate http://llvm.org/svn/llvm-project/compiler-rt/trunk https://$CNAME@llvm.org/svn/llvm-project/compiler-rt/trunk compiler-rt)

# Choose the host compiler
# Choose CMAKE_BUILD_TYPE {Debug, Release}
# Choose LLVM_ENABLE_ASSERTIONS {ON,OFF}
# Choose LLVM_ENABLE_WERROR {ON,OFF}
# Set LLVM_TARGETS_TO_BUILD to X86 to speed up the build]

mkdir llvm_cmake_build && cd llvm_cmake_build
# CC=gcc CXX=g++ cmake -DCMAKE_BUILD_TYPE=Debug -DLLVM_USE_LINKER=gold -DLLVM_ENABLE_ASSERTIONS=ON [-DLLVM_ENABLE_WERROR=ON] [-DLLVM_TARGETS_TO_BUILD=X86] /media/kyoupark/extended/llvm/llvm
CC=gcc CXX=g++ cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_USE_LINKER=gold -DLLVM_ENABLE_ASSERTIONS=ON [-DLLVM_ENABLE_WERROR=ON] [-DLLVM_TARGETS_TO_BUILD=X86] /home/kyoupark/asan/llvm

# Now you can build and test LLVM code:
# make -j12 # build everything
# make check-all -j12 # build and run all tests (llvm+clang+ASan)
# make check-sanitizer -j12 # build and run sanitizer_common tests
# make check-asan -j12 # build and run ASan unit and output tests
# Note that dynamic runtime tests are not included into check-asan or check-all on platforms where static runtime is default; to run them, you'll need to manually
# 
# make -j12 check-asan-dynamic
# To build only ASan runtime library:
# 
# # On Linux:
# make -j12 asan
# # The runtime libraries are built at:
# # llvm_cmake_build/lib/clang/<llvm_version>/lib/{linux,darwin}/
# To build/run only ASan unit tests:
# 
# make -j12 AsanUnitTests
# cd projects/compiler-rt/lib/asan/tests
# ./Asan-x86_64-Test
# ./Asan-i386-Test
# ./Asan-x86_64-Noinst-Test
# ./Asan-i386-Noinst-Test
# To re-run a single ASan lit-style output test make sure you've built Clang and run:
# 
# # Make sure clang and llvm tools are in your PATH.
# export PATH="/path/to/llvm_cmake_build/bin:$PATH"
# # Run a specific output test.
# cd llvm_cmake_build
# bin/llvm-lit projects/compiler-rt/test/asan/64bitConfig/TestCases/deep_tail_call.cc
# Visit LLVM CMake guide to see the ways you can customize your LLVM/Clang/ASan build. For example, you can build LLVM/Clang with ASan-ified Clang:
# mkdir llvm_cmake_build_asan
# cd llvm_cmake_build_asan
# export CC=clang
# export CXX=clang++
# cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_USE_SANITIZER=Address /path/to/llvm/checkout
# To run lint:
# 
# projects/compiler-rt/lib/sanitizer_common/scripts/check_lint.sh
# 
