#!/bin/bash

set -v

echo 1 > /sys/module/vfio/parameters/enable_unsafe_noiommu_mode
modprobe vfio-pci
modprobe vfio enable_unsafe_noiommu_mode=1

dpdk-devbind.py -b vfio-pci 0000:00:13.0
dpdk-devbind.py -b vfio-pci 0000:00:14.0
dpdk-devbind.py --status net
