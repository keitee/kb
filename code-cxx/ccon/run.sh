#/bim/bash

COUNT=0

while :
do
  echo ""
  echo "Looping $COUNT..."
  echo ""
  ./ccon_ex_out
  let COUNT=COUNT+1
done
