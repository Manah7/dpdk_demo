## DPDK tuto simple
L'idée est simplement de faire fonctionner DPDK dans une configuration simple, sans se préoccuper des configuration des drivers. Dans cet exemple, on utilise directement `testpmd` avec des *virtual devices* sur des interfaces physiques.

### Exemple simple passant par la stack Linux
```bash
echo 2048 > /proc/sys/vm/nr_hugepages

# Sans driver DPDK configurés
dpdk-testpmd -c7 --vdev=net_pcap0,iface=ens192 --vdev=net_pcap1,iface=ens224 -- -i --nb-cores=2 --nb-ports=2 --total-num-mbufs=2048

# Avec les drivers DPDK (la configuration des drivers n'est pas précisée ici)
dpdk-testpmd -c7 -- -i --nb-cores=2 --nb-ports=2 --total-num-mbufs=2048
```
Commandes utiles de `testpmd` :
```
start tx_first
show port stats all
show port info all
stop
```