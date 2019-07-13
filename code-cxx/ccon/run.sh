#/bim/bash

COUNT=0

while :
do
  echo ""
  echo "Looping $COUNT..."
  echo ""
  ./ccon_out --gtest_filter=*Counter*

  if [ $? -ne 0 ]; then
    echo "Failed..."
    exit
  fi 

  let COUNT=COUNT+1
done
