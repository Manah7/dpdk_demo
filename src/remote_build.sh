#!/bin/bash

set -e

# Profil SSH utilisé
HOST=dpdk-1

ssh $HOST -- rm -vfr src
scp -r ../src $HOST:
ssh $HOST -- "rm -vf src/remote_build.sh"
ssh $HOST -- "cd src && ls -alh && make && tree ."

if [ "$1" = "t" ]; then
    echo -e "\tPour lancer le programme :"
    echo -e "\tcd src/build/ && ./main -l 1 -n 4"
    ssh $HOST
fi
