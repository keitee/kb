
# On real build machine

# 10,000,000 ../numbers.in

1.6.1: qsort
real    0m5.315s
user    0m5.279s
sys     0m0.036s

1.6.1: stl
real    0m27.109s
user    0m26.601s
sys     0m0.506s

1.6.1: bit
real    0m3.410s
user    0m3.383s
sys     0m0.027s

1.6.1: stl sort, vector and sort(), vector.reserve() do not make difference.
real    0m43.999s
user    0m43.959s
sys     0m0.039s

# 1,000,000 ../numbers2.in

1.6.1: qsort
real    0m0.486s
user    0m0.478s
sys     0m0.008s

1.6.1: stl
real    0m1.706s
user    0m1.677s
sys     0m0.029s

1.6.1: bit
real    0m0.332s
user    0m0.327s
sys     0m0.005s

1.6.1: stl sort
real    0m3.768s
user    0m3.762s
sys     0m0.006s

# On VM
1.6.1: n: 10000000 library qsort
real    0m10.935s
user    0m10.852s
sys     0m0.052s

1.6.1: n: 10000000 stl set
real    0m36.295s
user    0m36.008s
sys     0m0.180s

sizeof(a): 1250004
1.6.1: n: 10000000 bit vector
real    0m7.961s
user    0m7.904s
sys     0m0.024s

1.6.1: n: 0 stl sort on vector
real    3m16.282s
user    3m15.136s
sys     0m0.236s

1.6.1: n: 10000000 stl sort on array
real    0m11.207s
user    0m11.140s
sys     0m0.040s

1.6.1: n: 10000000 quick sort on array
real    0m11.234s
user    0m11.168s
sys     0m0.024s

#
1.6.1: n: 1000000 library qsort
real    0m1.106s
user    0m1.092s
sys     0m0.008s

1.6.1: n: 1000000 stl set
real    0m3.382s
user    0m3.344s
sys     0m0.016s

sizeof(a): 1250004
1.6.1: n: 1000000 bit vector
real    0m1.056s
user    0m1.048s
sys     0m0.000s

1.6.1: n: 0 stl sort on vector
real    0m17.711s
user    0m17.608s
sys     0m0.040s

1.6.1: n: 1000000 stl sort on array
real    0m1.035s
user    0m1.032s
sys     0m0.000s

1.6.1: n: 1000000 quick sort on array
real    0m1.066s
user    0m1.060s
sys     0m0.000s

