#!/bin/bash

# Sur l'hote, AVANT le boot de la VM :
# sudo dpdk-testpmd -l 0,2,3,4,5 --socket-mem=1024 -n 4     --vdev 'net_vhost0,iface=/tmp/vhost-user1'     --vdev 'net_vhost1,iface=/tmp/vhost-user2' --     --portmask=f -i --rxq=1 --txq=1     --nb-cores=4 --forward-mode=io
# APRES ce script, sur l'hote : start tx-first

set -v

# Build
export PKG_CONFIG_PATH="/usr/local/lib64/pkgconfig"

# ModProbe driver 
modprobe vfio enable_unsafe_noiommu_mode=1
modprobe vfio-pci

# Huge page
echo 800 > /proc/sys/vm/nr_hugepages

dpdk-devbind -b vfio-pci 0000:07:00.0 0000:08:00.0

#testpmd -l 0,1,2 --socket-mem 1024 -n 4 \
#    --proc-type auto --file-prefix pg -- \
#    --portmask=3 --forward-mode=macswap --port-topology=chained \
#    --disable-rss -i --rxq=1 --txq=1 \
#    --rxd=256 --txd=256 --nb-cores=2 --auto-start