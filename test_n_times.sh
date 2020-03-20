#!/bin/bash

TIMES=$1
ERR_CNT=0

if [ -z $TIMES ] ;
then
	echo "need how many iterations."
	exit 1
fi

for ((i = 0 ; i < ${TIMES} ; i++)); 
do
	echo "Iteration: ${i}"
	echo "Bad runs: ${ERR_CNT}"
	./test/mesh/meshTests
	ERR=$?
	if [ $ERR -ne 0 ]; then
		ERR_CNT=$((ERR_CNT+1))
	fi
done

echo "RUN TIMES: ${TIMES}"
echo "ERRORS: $ERR_CNT"
if [ $ERR_CNT -gt 0 ];
then
	exit 1
fi
exit 0
