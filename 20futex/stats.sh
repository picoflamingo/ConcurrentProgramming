#!/bin/bash

if [ $# -ne 2 ]
then
   echo "Usage: stats.sh PROGRAM NUM_ITERATIONS"
   exit 1
fi

PRG=$1
N=$2


echo "Running $PRG $N times..."

truncate --size 0 /tmp/res
for ((i = 1; i < $N; i++))
do
    echo -n "."
     /usr/bin/time -f "%E %U %S %P" ./test05 1>/dev/null 2>> /tmp/res

done

echo
awk '{ sub(/%$/,"",$4); total1 += $2; total2 += $3; total4 += $4; count++} END { print total1/count, total2/count, total4/count}' /tmp/res
