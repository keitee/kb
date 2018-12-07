#/bim/bash

# 10,000,000 ../numbers.in
time ./s161-lib-qsort_out < ../numbers.in
time ./s161-stl-set_out < ../numbers.in
time ./s161-bit-array_out < ../numbers.in
time ./s161-stl-sort-vector_out < ../numbers.in
time ./s161-stl-sort-array_out < ../numbers.in
time ./s161-ppp-isort_out < ../numbers.in
time ./s161-ppp-qsort_out < ../numbers.in

# # 1,000,000 ../numbers2.in
# time ./s161-qsort_out < ../numbers2.in
# time ./s161-stl_out < ../numbers2.in
# time ./s161-bit_out < ../numbers2.in
# time ./s161-stl-sort_out < ../numbers2.in
# time ./s161-stl-sort-array_out < ../numbers2.in
# # time ./s161-isort_out < ../numbers2.in
# time ./s161-qsort-own_out < ../numbers2.in
