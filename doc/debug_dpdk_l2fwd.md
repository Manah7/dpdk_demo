## Debug de DPDK avec l2fwd
Ce fichier contient des informations qui nous ont été utiles pour comprendre le fonctionnement de l'exemple `l2fwd`, que nous avons finalement abandonné au profit de `dpdk-skeleton`.

### MAC list
* dpdk-0   : `c6:56:50:3e:40:84`
* dpdk-1/0 : `e6:7d:3e:f5:78:ca` (??? -> ok)
* dpdk-1/2 : `ce:33:73:2e:d4:2e` (??? -> ok)
* dpdk-2   : `56:a2:1d:e9:69:3f`

### Info
**ARP handshake avec testpmd**
```
09:15:51.319106 c6:56:50:3e:40:84 > ff:ff:ff:ff:ff:ff, ethertype ARP (0x0806), length 42: Request who-has 192.168.0.2 tell 192.168.0.1, length 28
09:15:51.319126 56:a2:1d:e9:69:3f > c6:56:50:3e:40:84, ethertype ARP (0x0806), length 42: Reply 192.168.0.2 is-at 56:a2:1d:e9:69:3f, length 28
```
**ARP fail avec l2fwd**
Avec `./build/l2fwd -c 1 -n 4 -- -q 8 -P -p f --portmap="(0,1)"` :
```
09:14:24.606076 ce:33:73:2e:d4:2e > 02:00:00:00:00:01, ethertype ARP (0x0806), length 42: Request who-has 192.168.0.2 tell 192.168.0.1, length 28
09:14:25.632874 ce:33:73:2e:d4:2e > 02:00:00:00:00:01, ethertype ARP (0x0806), length 42: Request who-has 192.168.0.2 tell 192.168.0.1, length 28
```
Depuis dpdk-2 :
```
09:26:33.341045 e6:7d:3e:f5:78:ca > 02:00:00:00:00:00, ethertype ARP (0x0806), length 42: Request who-has 192.168.0.1 tell 192.168.0.2, length 28
09:26:34.369726 e6:7d:3e:f5:78:ca > 02:00:00:00:00:00, ethertype ARP (0x0806), length 42: Request who-has 192.168.0.1 tell 192.168.0.2, length 28
```

***Il semblerait juste que l'application ne soit pas conçue pour ça. Utile pour tester les performances (cf. [kvm_l2fwd.md](kvm_l2fwd.md)) mais inutile en pratique.***

## Utilisation de dpdk-skeleton
`dpdk-skeleton` semble plus approprié pour notre application, prenant un paquet sur un port et le passant à l'autre, fonctionnelle :
```bash
./dpdk-skeleton -l 1 -n 4
```
La commutation à lieu ici :
```C
    /* Get burst of RX packets, from first port of pair. */
	struct rte_mbuf *bufs[BURST_SIZE];
	const uint16_t nb_rx = rte_eth_rx_burst(port, 0,
			bufs, BURST_SIZE);

	if (unlikely(nb_rx == 0))
		continue;

	/* Send burst of TX packets, to second port of pair. */
	const uint16_t nb_tx = rte_eth_tx_burst(port ^ 1, 0,
			bufs, nb_rx);
```