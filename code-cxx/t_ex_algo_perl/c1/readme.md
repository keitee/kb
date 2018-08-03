
# On real build machine

# 10,000,000 ../numbers.in

1.6.1: qsort
real    0m5.192s
user    0m5.018s
sys     0m0.174s

1.6.1: stl
real    0m27.502s
user    0m26.757s
sys     0m0.745s

1.6.1: bit
real    0m3.303s
user    0m3.202s
sys     0m0.101s

# 1,000,000 ../numbers2.in

1.6.1: qsort
real    0m0.544s
user    0m0.538s
sys     0m0.006s

1.6.1: stl
real    0m1.796s
user    0m1.749s
sys     0m0.047s

1.6.1: bit
real    0m0.338s
user    0m0.313s
sys     0m0.025s

# On VM

# 10,000,000 ../numbers.in

1.6.1: qsort
real    0m11.318s
user    0m11.164s
sys     0m0.064s

1.6.1: stl
real    0m40.574s
user    0m39.940s
sys     0m0.504s

1.6.1: bit
real    0m8.120s
user    0m8.048s
sys     0m0.024s

1.6.1: stl sort, vector and sort(), vector.reserve() do not make difference.
real    3m18.967s
user    3m18.248s
sys     0m0.156s

# 1,000,000 ../numbers2.in

1.6.1: qsort
real    0m1.102s
user    0m1.068s
sys     0m0.012s

1.6.1: stl, set
real    0m3.107s
user    0m3.064s
sys     0m0.032s

1.6.1: bit
real    0m0.834s
user    0m0.828s
sys     0m0.000s

1.6.1: stl sort, vector and sort(), vector.reserve() do not make difference.
real    0m18.328s
user    0m18.268s
sys     0m0.008s
