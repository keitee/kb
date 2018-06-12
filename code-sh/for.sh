# for sh

# ={=========================================================================
# sh-loop

echo "==================="
echo "use list "

for count in 0 1 2 3
do
  echo "looping.. $count"
  filename=S$count
  echo "filename: $filename.."
done

echo "==================="
echo "use seq "

for count in $(seq 4)
do
  echo "looping.. $count"
  filename=S$count
  echo "filename: $filename.."
done

echo "==================="
echo "use arithmetic "

COUNT=0

while [ $COUNT -lt 4 ]
do
  echo "looping.. $COUNT"
  filename=S$COUNT
  echo "filename: $filename.."
  let COUNT=COUNT+1
done

