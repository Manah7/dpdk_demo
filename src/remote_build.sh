#!/bin/bash

# Profil SSH utilisé
HOST=dpdk-1


ssh $HOST -- rm -vfr src
scp -r ../src $HOST:
ssh $HOST -- "cd src && ls -alh && make"

if [ "$1" = "t" ]; then
    echo -e "\tPour lancer le programme :"
    echo -e "\tcd src/build/ && ./main -l 1 -n 4"
    ssh $HOST
fi
