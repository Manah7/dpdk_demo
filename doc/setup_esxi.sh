#!/bin/bash

set -v

if [ "$#" -ne 2 ]; then
    pci=0000:0b:00.0
    pcj=0000:13:00.0
else
    pci=$1
    pcj=$2
fi

echo "On utilise $pci et $pcj"

echo 1 > /sys/module/vfio/parameters/enable_unsafe_noiommu_mode
echo 1024 > /proc/sys/vm/nr_hugepages
modprobe vfio-pci
modprobe vfio enable_unsafe_noiommu_mode=1

dpdk-devbind.py -b vfio-pci $pci
dpdk-devbind.py -b vfio-pci $pcj
dpdk-devbind.py --status net
