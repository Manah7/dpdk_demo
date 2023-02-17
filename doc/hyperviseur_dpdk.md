## DPDK dans une infrastructure virtuelle
Nous avons essayé de réaliser plusieurs bancs d'essais virtuels sous différents hyperviseurs. Le seul où nous avons obtenus des résultats significatifs est Proxmox. La structure utilisée pour le banc d'essais est la suivante :

![../img/schema_virt_dpdk.png](../img/schema_virt_dpdk.png)

### VMWare (Useless)
Bien que le déploiement des VM et la mise en place des vSwitch a été facile, la configuration de DPDK des interfaces `vmxnet3` de VMWare n'a pas permit d'utiliser DPDK correctement, en particulier DPDK ne semblait pas recevoir certains paquets, malgré le mode promiscous du switch.

Commandes utilisées pour le debug :
```bash
tcpdump -i ens224 -vv -en

make
./l2fwd -c 1 -n 4 -- -p 2
./l2fwd -c 1 -n 4 -- -q 8 -p f --portmap="(0,1)"

dpdk-testpmd -c7 -n 4 -- -i --nb-cores=2 --nb-ports=2 --total-num-mbufs=2048
```

### Proxmox
J'ai eu plus de succès avec Proxmox, même si les performances restent inférieures qu'`iptables`. Pour la configuration, outre l'installation des DPDK sur dpdk-1, j'utilise un script similaire à celui de [kvm_l2fwd.md](kvm_l2fwd.md) :
```bash
#!/bin/bash

set -v

echo 1 > /sys/module/vfio/parameters/enable_unsafe_noiommu_mode
modprobe vfio-pci
modprobe vfio enable_unsafe_noiommu_mode=1

dpdk-devbind.py -b vfio-pci 0000:00:13.0
dpdk-devbind.py -b vfio-pci 0000:00:14.0
dpdk-devbind.py --status net
```
Je peux alors lancer l'exemple `dpdk-skeleton` :
```
root@dpdk-1:~# cd src/build/ && ./main -l 1 -n 4
EAL: Detected CPU lcores: 4
EAL: Detected NUMA nodes: 1
EAL: Detected shared linkage of DPDK
EAL: Multi-process socket /var/run/dpdk/rte/mp_socket
EAL: Selected IOVA mode 'PA'
EAL: No available 2048 kB hugepages reported
EAL: VFIO support initialized
EAL: Probe PCI driver: net_virtio (1af4:1000) device: 0000:00:12.0 (socket 0)
eth_virtio_pci_init(): Failed to init PCI device
EAL: Requested device 0000:00:12.0 cannot be used
EAL: Probe PCI driver: net_virtio (1af4:1000) device: 0000:00:13.0 (socket 0)
EAL: Using IOMMU type 8 (No-IOMMU)
EAL: Ignore mapping IO port bar(0)
EAL: Probe PCI driver: net_virtio (1af4:1000) device: 0000:00:14.0 (socket 0)
EAL: Ignore mapping IO port bar(0)
TELEMETRY: No legacy callbacks, legacy socket not created
Port 0 MAC: e6 7d 3e f5 78 ca
Port 1 MAC: ce 33 73 2e d4 2e

Core 1 forwarding packets. [Ctrl+C to quit]
```
Pour les performances obtenues, voir [perf_dpdk_promox.md](perf_dpdk_promox.md).

### Généralitées
```bash
# Accès aux VM
for i in $(seq 0 2); do gnome-terminal -- ssh dpdk@192.168.200.2$i; done

# CSSH
cssh --fillscreen  dpdk-0 dpdk-1 dpdk-2 dpdk-3
```

