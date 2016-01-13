#!/bin/bash
#$ -clear
#$ -S /bin/sh
#$ -w w
#$ -cwd
#$ -q 4c.q
#$ -j y
# get rid of this #$ -v PATH=$path

ROOT=$(pwd)

export LD_LIBRARY_PATH=${ROOT}/vendor/zlog/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=${ROOT}/vendor/igraph/lib:$LD_LIBRARY_PATH
#valgrind --leak-check=full 
./Debug/graphtocluster -c ./graphtocluster.cnf

