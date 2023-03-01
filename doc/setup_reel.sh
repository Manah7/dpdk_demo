#!/bin/bash

set -v

echo 1 > /sys/module/vfio/parameters/enable_unsafe_noiommu_mode

modprobe vfio-pci
modprobe vfio enable_unsafe_noiommu_mode=1

modprobe uio
insmod /root/libmoon/deps/dpdk/x86_64-native-linuxapp-gcc/kmod/igb_uio.ko

# utilisation de igb_uio
dpdk-devbind.py -b igb_uio 0000:04:00.0
dpdk-devbind.py -b igb_uio 0000:04:00.1

dpdk-devbind.py --status net
