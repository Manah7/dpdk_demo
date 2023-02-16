#!/bin/bash

HOST=dpdk-1

ssh $HOST -- rm -vfr src
scp -r ../src $HOST:
ssh $HOST -- "cd src && ls -alh && make"

if [ "$1" = "t" ]; then
    echo "Pour lancer le programme :"
    echo "cd src/build/ && ./main -l 1 -n 4"
    ssh $HOST
fi
