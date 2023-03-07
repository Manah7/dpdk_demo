#!/bin/bash

# Usage :
# ./remote_build.sh [-h] [-t] [-d] [-i ssh_config]
#   -t      Ouvre un terminal sur la machine distante après la compilation
#   -i      Précise le profil SSH à utiliser
#   -d      Active le debug (très verbeux)
#   -h      Affiche l'aide


## Variables
POSITIONAL_ARGS=()
TERMINAL=0
DEBUG=0

### Profil SSH utilisé
SSH_ID=dpdk-1


## Arguments
while [[ $# -gt 0 ]]; do
  case $1 in
    -i|--identity)
      ID="$2"
      shift
      shift
      ;;
    -t|--terminal)
      TERMINAL=1
      shift
      ;;
    -d|--debug)
      DEBUG=1
      shift
      ;;
    -h)
      echo -e "Usage : $0 [-h] [-t] [-d] [-i ssh_config]"
      exit 0
      ;;
    --help)
      head -n 9 $0 | tail -n 6
      exit 0
      ;;
    -*|--*)
      echo "Parametre inconnu : $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") 
      shift
      ;;
  esac
done

set -- "${POSITIONAL_ARGS[@]}" # restore positional parameters

if [ "$TERMINAL" -eq "1" ]; then
   echo "Opening terminal after build.";
fi

if [ -z $ID ]; then 
    echo "No SSH identity provided, using default: $SSH_ID"; 
else 
    SSH_ID=$ID
    echo "Using given SSH identity: $SSH_ID"; 
fi

if [ "$DEBUG" -eq "1" ]; then
   echo "Debug mode. Very verbose.";
fi

echo ""

## Opérations
ssh $SSH_ID -- rm -vfr src
scp -r ../src $SSH_ID:
ssh $SSH_ID -- "rm -vf src/remote_build.sh"

if [ "$DEBUG" -eq "1" ]; then
    ssh $SSH_ID -- "cd src && ls -alh && make debug && tree ."
else
    ssh $SSH_ID -- "cd src && ls -alh && make && tree ."
fi

if [ "$TERMINAL" -eq "1" ]; then
    echo -e "\tPour lancer le programme :"
    echo -e "\tcd src/build/ && ./main -l 1 -n 4"
    ssh $SSH_ID
fi

exit 0
