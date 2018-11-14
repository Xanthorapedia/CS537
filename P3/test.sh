#!/bin/bash

for i in ./*; do
	echo $i;
	if [ -d "$i" ]; then
		cd $i;
		valgrind --leak-check=yes --track-origins=yes --read-var-info=yes ../../537make
		cd ..;
	fi
done
