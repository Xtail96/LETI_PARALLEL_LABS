#!/bin/bash

case ${1} in
	'build') 
		mpic++ -std=c++11 -o ${2}.out ${2}.cpp;;
	'run')
		mpirun --hostfile hostfile -np ${3} ${2}.out;;
	'launch')
		mpic++ -std=c++11 -o ${2}.out ${2}.cpp
		mpirun --hostfile hostfile -np ${3} ${2}.out;;
esac	