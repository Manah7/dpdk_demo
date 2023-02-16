## KVM l2fwd
*C'est la suite de [kvm_dpdk.md](kvm_dpdk.md).*

Le but est de faire fonctionner le forwarding non plus avec `testpmd`, mais avec l'exemple `l2fwd` de DPDK. Cela nous permettra de tester directement avec du code C modifiable et avec lequel on peut travailler.

### Build DPDK et l2fwd sur le CentOS 7
```bash
# Prérequis
yum install dpdk-devel python3-pip
pip3 install meson ninja pyelftools
export PKG_CONFIG_PATH="/usr/local/lib64/pkgconfig"

# Installation des sources
wget https://fast.dpdk.org/rel/dpdk-22.11.1.tar.xz
tar -xf dpdk-22.11.1.tar.xz
mv dpdk-22.11.1 dpdk

# Build de DPDK
meson build
ninja -C build
ninja -C build install

# Build de l2fwd
cd dpdk/examples/l2fwd
make

# On éteint la VM pour lancer le test
poweroff
```

### Utilisation de l'exemple
On reprend la même configuration qu'avec `testpmd`, en l'adaptant sur l'hôte : 
```bash
sudo dpdk-testpmd -l 0,2,3,4,5 --socket-mem=1024 -n 4 --vdev 'net_vhost0,iface=/tmp/vhost-user1' --vdev 'net_vhost1,iface=/tmp/vhost-user2' -- --portmask=0x3 --port-topology=chained --disable-rss -i --rxq=1 --txq=1 --nb-cores=4 --forward-mode=macswap
```
On lance ensuite la VM et on s'y connecte (dans un nouveau terminal) :
```bash
virsh start vhuser-test1
ssh root@192.168.122.210
```
On lance le test côté VM :
```bash
./setup.sh # Script utilisé précédement, voir annexe
cd dpdk/examples/l2fwd/build
./l2fwd -c 1 -n 4 -- -q 8 -p f --portmap="(0,1)"
```
Puis côté hôte :
```
start tx_first
```

On doit alors voir un forward de paquet :
```
Port statistics ====================================
Statistics for port 0 ------------------------------
Packets sent:                 12572512
Packets received:             12721184
Packets dropped:                     0
Statistics for port 1 ------------------------------
Packets sent:                 12721184
Packets received:             12572512
Packets dropped:                     0
Aggregate statistics ===============================
Total packets sent:           25293696
Total packets received:       25293696
Total packets dropped:               0
====================================================
```

## Annexe
### Script de setup
```bash
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
```

