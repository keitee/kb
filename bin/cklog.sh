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
a79591e085ea04a6d54a5cfd367f7b33
d64c536826454b7a36697f7bcb948f77
6e74a9e6373239e7548c9e4e9ac0f462
f33491d4392f23bd82a8e0034c74292f
3d8ae96d3359ee4c8725f87b1b1d0ea5
1fa88de0d93c704d2407c90c0fd3e7ae
349076fc17e6993de518e34f76dd7342
33e549f7763a9de3ee07a241331db051
b7685f058cf4e2d3b043721d1fccbe29
b83269ebc4e7b048998e7a5b722766cd
04795750415b9f27c84b26ec57f1b942
1442edcd55d94d2c7630ee87dfb3333e
50cd2337922fab3bbf3790ecdc6dc92c
c67cb5318bea777b72329922a6806ca5
d3681d47198bbd63ec30460f4febb3cf
5cdeead732a040fdee787438b573360b
6a5cfdc4f9d7d4eb85ed6d1a38e8c26c
60d5767807308f444c1b1db91df7efca
274a169df65d32d26973414cd678055f
e09e4b73e8569dd6c1ced7361ee02c04
02222a0076a4283ecd4101b661aeaba1
1bd7ee29f243453ed02139c1fba85729
5cd13c2c28eca48409ed3bea90c4f05d
b3dad8316ba15b5e434bef52dbe03960
b207b85adda50e366f0f95f88ebab7f6
d64def07041792ddd186423ad98cb58d
6043b9752c8c19b589ff1100f02ef227
c29a3f0fc87cf6f825cab63e86ad4271
414d78bc48929f5441a0b40ec7c454bb
808a9a9fb8851528777b40b3faf5af10
d5fc5de18ad709d08fa9445cfdc0f6e8
714c299a9b15b0f07862ad14df742eaa
65edd8ffdcd4ff7b859869e561cacb55
59b18249ecb675817f30653ff780e3a9
732459799404029dc372841ee09c4204
ec5cfbe6b8fe37b707bda145cb27d89e
66d32596d6a816f2070e4116b42d06f6
23ce7a1815a837960251cd7bf75f676b
acd58370a4f873c45888baee2c5857eb
1ba390bfa3bb30da94b11adecf0d83f6
3ce683e48e5ca64f36a91d02ca490b3c
)

#
for item in ${list[@]}; do ssh yard /home/si_logs/tmp_kit/worker_cklog.sh ${item}; done
