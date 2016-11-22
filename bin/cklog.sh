#!/bin/bash

# declare -ar cwlist=(
#   # # Zone2 - box36 
#   # 0019fb2c55e9
#   # # Zone6 - Box36
#   # 0019fb2d3660
#   # # Zone3 - Box36 
#   # 0019fb2c5608
#   # # Zone2 - Box35
#   # 0019fb2c55ec
#   # # Zone6 - Box35
#   # 0019fb2d3850
#   # Zone3 - Box34
#   # 00:19:fb:2d:37:c0
#   # Zone4 - Box34
#   # 00:19:fb:2c:55:cd
#   # Zone2 - Box34
#   00:19:fb:2c:56:11
# )
# 
# # list_log 0019fb2d3660 | grep R014.083.60.00C | cut -f 4 | cut -f 1 -d'.' | xargs check_log
# 
# version=R014.083.60.00C
# 
# for item in ${cwlist[@]}; do echo "===="; 
#   echo "item is ${item}..."
#   # list_log can accepts mac.
#   # found is darwin_0019fb2d3660_e1a3726db88bc9ca9e363413ec97ecdd
#   found=`ssh theyard list_log ${item} | grep ${version} | cut -f 4 | cut -f 1 -d'.'`
#   # check_log cannot find some but check_log web does. ??
#   echo "found is ${found}..."
#   # ssh theyard check_log ${found}
# done
# 
# exit 0
# 
# declare -ar list=(
#   translation/darwin_0019fbf2b6a1_09f5479d8fe07e2a0dec70e36c4077ef
#   translation/darwin_783e53042058_a24511f64b0fba05ed1087b818f1895a
#   translation/darwin_0019fb38e30a_5798cc883511d82578025835cc9a2cf0
#   translation/darwin_0019fb0f53b4_b558b7a71573867c0b68cb77bfb97e57
#   translation/darwin_0019fbd5a013_da9ceaca66653c6433936ea62ddb0a21
#   translation/darwin_0019fbd18c68_a3d4781b9564b2b3d59838ea196ce12f
#   translation/darwin_0019fb8817bf_d812975532385d4d93c2c1ae2b36ea43
#   translation/darwin_0019fbf2b6d2_70ccc70f8a1b294ba94306aed322793c
#   translation/darwin_783e5303e765_29e4bc760c9a09aa8899f5c38fe8ff09
#   translation/darwin_0019fb384838_c54a20fc79fe47ed001742e9d2cfa583
#   translation/darwin_0019fb0f4cbc_f5864abdffae856f056058a516d12b44
#   translation/darwin_0019fbaba3a2_1b88f8981b449cb32f6e3b77a66e31fb
#   translation/darwin_0019fbd18c9a_487032cd821312aea3b11274c2ff7384
#   translation/darwin_0019fbb71c47_c4130c208206b80b3963ee05c861b5c3
#   translation/darwin_0019fbb71bb1_d23afbe329e75a1d9bcabc233373d398
# )
# 
# for item in ${list[@]}; do echo "==="; echo ${item}; pushd ${item}; echo "-> `pwd`"; cat INFO.TXT; egrep -anH "Leak de|Could not unravel" LOGlastrun_realtime;popd; done
# 
# # for item in ${list[@]}; do echo ${item}; pushd ${item};egrep -anH "Leak de|Could not unravel" LOGlastrun_realtime;popd; done

declare -ar list=(
c1e32f74d48e00003516c3e939be26c1
fc5797be432bd5db3ec23be1305dd33c
6611afdf91df1b43c2f5e3a8f63b1643
)

#
for item in ${list[@]}; do ssh theyard /home/si_logs/tmp_kit/worker_cklog.sh ${item}; done
