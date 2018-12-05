#!/bin/bash

make clean obj
cd ./testcases
rm -f ./testcases/*.[oh]
cp ./*.h ./537malloc.o .
make clean all
for i in ./*.out
do
	echo Testing: $i
	$i 1>/dev/null # comment 1>/dev/null to print stdout
done
