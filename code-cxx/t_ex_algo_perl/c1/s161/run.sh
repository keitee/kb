#/bim/bash

# 10,000,000
time ./s161-qsort_test < ../numbers.in
time ./s161-stl_test < ../numbers.in
time ./s161-bit_test < ../numbers.in

# 1,000,000
time ./s161-qsort_test < ../numbers2.in
time ./s161-stl_test < ../numbers2.in
time ./s161-bit_test < ../numbers2.in

