#!/bin/bash

# Usage :
# ./remote_build.sh [-t] [-r] [-i ssh_config]
#   -t      Ouvre un terminal sur la machine distante après la compilation
#   -r      Utilise l'hôte remote (enregistré dans la configuration)
#   -i      Précise le profil SSH à utiliser


## Variables
POSITIONAL_ARGS=()
REMOTE=0
TERMINAL=0

### Profil SSH utilisé
SSH_ID=dpdk-1
SSH_RT=remote-dpdk-1


## Arguments
while [[ $# -gt 0 ]]; do
  case $1 in
    -i|--identity)
      ID="$2"
      shift
      shift
      ;;
    -r|--remote)
      REMOTE=1
      shift
      ;;
    -t|--terminal)
      TERMINAL=1
      shift
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

if [ "$REMOTE" -eq "1" ]; then
   echo "Remote build.";
   SSH_ID=$SSH_RT
fi

if [ "$TERMINAL" -eq "1" ]; then
   echo "Opening terminal after build.";
fi

if [ -z $ID ]; then 
    echo "No SSH identity provided, using default: $SSH_ID"; 
else 
    SSH_ID=$ID
    echo "Using given SSH identity: $SSH_ID"; 
fi

echo ""

## Opérations
ssh $SSH_ID -- rm -vfr src
scp -r ../src $SSH_ID:
ssh $SSH_ID -- "rm -vf src/remote_build.sh"
ssh $SSH_ID -- "cd src && ls -alh && make && tree ."

if [ "$TERMINAL" -eq "1" ]; then
    echo -e "\tPour lancer le programme :"
    echo -e "\tcd src/build/ && ./main -l 1 -n 4"
    ssh $SSH_ID
fi

exit 0
