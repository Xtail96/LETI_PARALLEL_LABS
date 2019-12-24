#!/bin/bash

case ${1} in
	'build') 
		mpicc -o ${2}.out ${2}.c;;
	'run')
		mpirun --hostfile hostfile -np ${3} ${2}.out;;
	'launch')
		mpicc -o ${2}.out ${2}.c
		#mpirun --hostfile hostfile -np ${3} ${2}.out;;
		mpirun --hostfile hostfile -n ${3} --oversubscribe ${2}.out;;
esac