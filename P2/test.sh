#!/bin/sh

testdir="/u/c/s/cs537-1/public/proj2"

for testcase in $testdir/*
do
	echo "$testcase"
	exp=$(cat < "$testcase" | wc -l)
	act=$(prodcomm < "$testcase" 2>&1 | wc -l)
	echo "diff: $(( act-exp )), expected: $exp, actual: $act"
done

