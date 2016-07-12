#!/bin/bash

declare -ar list=(
  darwin_0019fbf2b6a1_09f5479d8fe07e2a0dec70e36c4077ef.tgz
  darwin_783e53042058_a24511f64b0fba05ed1087b818f1895a.tgz
  darwin_0019fb38e30a_5798cc883511d82578025835cc9a2cf0.tgz
  darwin_0019fb0f53b4_b558b7a71573867c0b68cb77bfb97e57.tgz
  darwin_0019fbd5a013_da9ceaca66653c6433936ea62ddb0a21.tgz
  darwin_0019fbd18c68_a3d4781b9564b2b3d59838ea196ce12f.tgz
  darwin_0019fb8817bf_d812975532385d4d93c2c1ae2b36ea43.tgz
  darwin_0019fbf2b6d2_70ccc70f8a1b294ba94306aed322793c.tgz
  darwin_783e5303e765_29e4bc760c9a09aa8899f5c38fe8ff09.tgz
  darwin_0019fb384838_c54a20fc79fe47ed001742e9d2cfa583.tgz
  darwin_0019fb0f4cbc_f5864abdffae856f056058a516d12b44.tgz
  darwin_0019fbaba3a2_1b88f8981b449cb32f6e3b77a66e31fb.tgz
  darwin_0019fbd18c9a_487032cd821312aea3b11274c2ff7384.tgz
  darwin_0019fbb71c47_c4130c208206b80b3963ee05c861b5c3.tgz
  darwin_0019fbb71bb1_d23afbe329e75a1d9bcabc233373d398.tgz 
)

declare -ar list=(
  # http://theyard.cisco.com/si_logs/translation/darwin_0019fbf2b6a1_09f5479d8fe07e2a0dec70e36c4077ef/index.html
  # http://theyard.cisco.com/si_logs/translation/darwin_783e53042058_a24511f64b0fba05ed1087b818f1895a/index.html
  # http://theyard.cisco.com/si_logs/translation/darwin_0019fb38e30a_5798cc883511d82578025835cc9a2cf0/index.html
  # http://theyard.cisco.com/si_logs/translation/darwin_0019fb0f53b4_b558b7a71573867c0b68cb77bfb97e57/index.html
  # http://theyard.cisco.com/si_logs/translation/darwin_0019fbd5a013_da9ceaca66653c6433936ea62ddb0a21/index.html
  # http://theyard.cisco.com/si_logs/translation/darwin_0019fbd18c68_a3d4781b9564b2b3d59838ea196ce12f/index.html
  # http://theyard.cisco.com/si_logs/translation/darwin_0019fb8817bf_d812975532385d4d93c2c1ae2b36ea43/index.html
  # http://theyard.cisco.com/si_logs/translation/darwin_0019fbf2b6d2_70ccc70f8a1b294ba94306aed322793c/index.html
  # http://theyard.cisco.com/si_logs/translation/darwin_783e5303e765_29e4bc760c9a09aa8899f5c38fe8ff09/index.html
  # http://theyard.cisco.com/si_logs/translation/darwin_0019fb384838_c54a20fc79fe47ed001742e9d2cfa583/index.html
  # http://theyard.cisco.com/si_logs/translation/darwin_0019fb0f4cbc_f5864abdffae856f056058a516d12b44/index.html
  # http://theyard.cisco.com/si_logs/translation/darwin_0019fbaba3a2_1b88f8981b449cb32f6e3b77a66e31fb/index.html
  # http://theyard.cisco.com/si_logs/translation/darwin_0019fbd18c9a_487032cd821312aea3b11274c2ff7384/index.html
  # http://theyard.cisco.com/si_logs/translation/darwin_0019fbb71c47_c4130c208206b80b3963ee05c861b5c3/index.html
  # http://theyard.cisco.com/si_logs/translation/darwin_0019fbb71bb1_d23afbe329e75a1d9bcabc233373d398/index.html
  translation/darwin_0019fbf2b6a1_09f5479d8fe07e2a0dec70e36c4077ef
  translation/darwin_783e53042058_a24511f64b0fba05ed1087b818f1895a
  translation/darwin_0019fb38e30a_5798cc883511d82578025835cc9a2cf0
  translation/darwin_0019fb0f53b4_b558b7a71573867c0b68cb77bfb97e57
  translation/darwin_0019fbd5a013_da9ceaca66653c6433936ea62ddb0a21
  translation/darwin_0019fbd18c68_a3d4781b9564b2b3d59838ea196ce12f
  translation/darwin_0019fb8817bf_d812975532385d4d93c2c1ae2b36ea43
  translation/darwin_0019fbf2b6d2_70ccc70f8a1b294ba94306aed322793c
  translation/darwin_783e5303e765_29e4bc760c9a09aa8899f5c38fe8ff09
  translation/darwin_0019fb384838_c54a20fc79fe47ed001742e9d2cfa583
  translation/darwin_0019fb0f4cbc_f5864abdffae856f056058a516d12b44
  translation/darwin_0019fbaba3a2_1b88f8981b449cb32f6e3b77a66e31fb
  translation/darwin_0019fbd18c9a_487032cd821312aea3b11274c2ff7384
  translation/darwin_0019fbb71c47_c4130c208206b80b3963ee05c861b5c3
  translation/darwin_0019fbb71bb1_d23afbe329e75a1d9bcabc233373d398
)

for item in ${list[@]}; do echo "==="; echo ${item}; pushd ${item}; echo "-> `pwd`"; cat INFO.TXT; egrep -anH "Leak de|Could not unravel" LOGlastrun_realtime;popd; done

# for item in ${list[@]}; do echo ${item}; pushd ${item};egrep -anH "Leak de|Could not unravel" LOGlastrun_realtime;popd; done

# declare -ar list=(
#   0019fb2c562c
#   0019fb2c5624
#   0019fbc3572c
#   0019fbe9554c
#   0019fbd76222
# )

for item in ${list[@]}; do echo "===="; echo ${item}; ssh theyard check_log ${item}; done
