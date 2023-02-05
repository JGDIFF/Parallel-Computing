#!/bin/bash
tests=('-h 200 -w 200' '-h 2000 -w 2000' '-h 5000 -w 4000' '-h 4000 -w 8000')


for atest in "${tests[@]}"
do
   seqArg="$atest -o seqMB.jpg"
   ./seqMB $seqArg > seqOutput

   staticArg="$atest -o staticMB.jpg"
   echo "Testing mpirun -np 5 ./staticParaMB $staticArg"
   mpirun -np 5 ./staticParaMB $staticArg > staticOutput

   diff seqMB.jpg staticMB.jpg > diffs
   if  [ ! -e staticMB.jpg ] || [ -s diffs ]; then
      echo "Test failed"
      echo "Output did not match seqMB output"
      rm -f diffs staticMB.jpg seqMB.jpg staticOutput seqOutput
      exit
   fi
done
rm -f diffs staticMB.jpg seqMB.jpg staticOutput seqOutput
echo "All tests of ./staticParaMB passed"

