#!/bin/bash

case ${1} in
	'build') 
		mpicc -o ${2}.out ${2}.c;;
	'run')
		mpirun --hostfile hostfile -np ${3} ${2}.out;;
	'launch')
		mpicc -o ${2}.out ${2}.c
		mpirun --hostfile hostfile -np ${3} ${2}.out;;
esac

#mpic++ -std=c++11 lab4.cpp  -lboost_mpi -lboost_serialization
#mpic++ -std=c++11 -o lab4.out  lab4.cpp