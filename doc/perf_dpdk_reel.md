## Performances de DPDK sur un banc d'essais réel
La forme du banc d'essais est la même que pour la partie virtualisée. Plusieurs paramètres ont été ajustés dans la situation réel, notamment au niveau du noyau Linux :
```
GRUB_CMDLINE_LINUX="iommu=pt intel_iommu=on default_hugepagesz=1G hugepagesz=1G hugepages=16 isolcpus=2,3 nohz_full=2,3 rcu_nocbs=2,3"
```
Ces choix ont été faits à l'aide du [benchmark intel](https://fast.dpdk.org/doc/perf/DPDK_22_11_Intel_NIC_performance_report.pdf), mais ne semblent pas améliorer significativement les performances.

### Résultats

Test avec `dpdk-testpmd -l1,2,3 -- -i --txd=2048 --rxd=2048 --forward=io -a` :
```
dpdkuser@DPDK-one:~$ iperf3 -c 192.168.129.2 
Connecting to host 192.168.129.2, port 5201
[  5] local 192.168.129.1 port 35804 connected to 192.168.129.2 port 5201
[ ID] Interval           Transfer     Bitrate         Retr  Cwnd
[  5]   0.00-1.00   sec   930 MBytes  7.80 Gbits/sec   35   3.10 MBytes       
[  5]   1.00-2.00   sec   938 MBytes  7.87 Gbits/sec  129   3.10 MBytes       
[  5]   2.00-3.00   sec   941 MBytes  7.90 Gbits/sec    0   3.10 MBytes       
[  5]   3.00-4.00   sec   935 MBytes  7.84 Gbits/sec  111   3.10 MBytes       
[  5]   4.00-5.00   sec   936 MBytes  7.85 Gbits/sec    0   3.10 MBytes       
[  5]   5.00-6.00   sec   940 MBytes  7.89 Gbits/sec    2   3.10 MBytes       
[  5]   6.00-7.00   sec   940 MBytes  7.89 Gbits/sec   52   3.10 MBytes       
[  5]   7.00-8.00   sec   935 MBytes  7.84 Gbits/sec    0   3.10 MBytes       
[  5]   8.00-9.00   sec   936 MBytes  7.86 Gbits/sec   52   3.10 MBytes       
[  5]   9.00-10.00  sec   971 MBytes  8.15 Gbits/sec    0   3.10 MBytes       
- - - - - - - - - - - - - - - - - - - - - - - - -
[ ID] Interval           Transfer     Bitrate         Retr
[  5]   0.00-10.00  sec  9.18 GBytes  7.89 Gbits/sec  381             sender
[  5]   0.00-10.04  sec  9.18 GBytes  7.86 Gbits/sec                  receiver

iperf Done.
```
**Dans l'autre sens...:**
```
dpdkuser@DPDK-two:~$ iperf3 -c 192.168.129.1
Connecting to host 192.168.129.1, port 5201
[  5] local 192.168.129.2 port 40460 connected to 192.168.129.1 port 5201
[ ID] Interval           Transfer     Bitrate         Retr  Cwnd
[  5]   0.00-1.00   sec  1.09 GBytes  9.32 Gbits/sec  1084   1.27 MBytes       
[  5]   1.00-2.00   sec  1.09 GBytes  9.40 Gbits/sec  392   1.44 MBytes       
[  5]   2.00-3.00   sec  1.09 GBytes  9.40 Gbits/sec  234   1.44 MBytes       
[  5]   3.00-4.00   sec  1.09 GBytes  9.40 Gbits/sec  165   1.50 MBytes       
[  5]   4.00-5.00   sec  1.09 GBytes  9.38 Gbits/sec   78   1.60 MBytes       
[  5]   5.00-6.00   sec  1.09 GBytes  9.39 Gbits/sec  238   1.52 MBytes       
[  5]   6.00-7.00   sec  1.09 GBytes  9.40 Gbits/sec  321   1.49 MBytes       
[  5]   7.00-8.00   sec  1.09 GBytes  9.41 Gbits/sec  328   1.48 MBytes       
[  5]   8.00-9.00   sec  1.07 GBytes  9.23 Gbits/sec  456   1.43 MBytes       
[  5]   9.00-10.00  sec  1.09 GBytes  9.37 Gbits/sec   38   1.53 MBytes       
- - - - - - - - - - - - - - - - - - - - - - - - -
[ ID] Interval           Transfer     Bitrate         Retr
[  5]   0.00-10.00  sec  10.9 GBytes  9.37 Gbits/sec  3334             sender
[  5]   0.00-10.04  sec  10.9 GBytes  9.33 Gbits/sec                  receiver

iperf Done.
```

Test avec `./main -l2,3` :
```
dpdkuser@DPDK-one:~$ iperf3 -c 192.168.129.2 
Connecting to host 192.168.129.2, port 5201
[  5] local 192.168.129.1 port 47626 connected to 192.168.129.2 port 5201
[ ID] Interval           Transfer     Bitrate         Retr  Cwnd
[  5]   0.00-1.00   sec   963 MBytes  8.08 Gbits/sec  208   3.01 MBytes       
[  5]   1.00-2.00   sec   968 MBytes  8.12 Gbits/sec   15   3.01 MBytes       
[  5]   2.00-3.00   sec   935 MBytes  7.85 Gbits/sec    0   3.01 MBytes       
[  5]   3.00-4.00   sec   936 MBytes  7.85 Gbits/sec    0   3.01 MBytes       
[  5]   4.00-5.00   sec   934 MBytes  7.83 Gbits/sec   33   3.01 MBytes       
[  5]   5.00-6.00   sec   925 MBytes  7.76 Gbits/sec  1625   2.16 MBytes       
[  5]   6.00-7.00   sec   945 MBytes  7.93 Gbits/sec   35   2.44 MBytes       
[  5]   7.00-8.00   sec   892 MBytes  7.49 Gbits/sec    0   2.69 MBytes       
[  5]   8.00-9.00   sec   931 MBytes  7.81 Gbits/sec    0   2.93 MBytes       
[  5]   9.00-10.00  sec   938 MBytes  7.86 Gbits/sec    0   3.00 MBytes       
- - - - - - - - - - - - - - - - - - - - - - - - -
[ ID] Interval           Transfer     Bitrate         Retr
[  5]   0.00-10.00  sec  9.15 GBytes  7.86 Gbits/sec  1916             sender
[  5]   0.00-10.04  sec  9.15 GBytes  7.82 Gbits/sec                  receiver

iperf Done.
```
**Dans l'autre sens :**
```
dpdkuser@DPDK-two:~$ iperf3 -c 192.168.129.1
Connecting to host 192.168.129.1, port 5201
[  5] local 192.168.129.2 port 57198 connected to 192.168.129.1 port 5201
[ ID] Interval           Transfer     Bitrate         Retr  Cwnd
[  5]   0.00-1.00   sec  1.09 GBytes  9.38 Gbits/sec  1890   3.13 MBytes       
[  5]   1.00-2.00   sec  1.09 GBytes  9.38 Gbits/sec  1973   2.48 MBytes       
[  5]   2.00-3.00   sec  1.09 GBytes  9.38 Gbits/sec  1495   2.73 MBytes       
[  5]   3.00-4.00   sec  1.09 GBytes  9.37 Gbits/sec  3236   2.96 MBytes       
[  5]   4.00-5.00   sec  1.06 GBytes  9.14 Gbits/sec  1868   1.68 MBytes       
[  5]   5.00-6.00   sec  1.09 GBytes  9.34 Gbits/sec    1   1.62 MBytes       
[  5]   6.00-7.00   sec  1.07 GBytes  9.23 Gbits/sec    4   1.51 MBytes       
[  5]   7.00-8.00   sec  1.08 GBytes  9.29 Gbits/sec  305   1.49 MBytes       
[  5]   8.00-9.00   sec  1.09 GBytes  9.40 Gbits/sec   34   1.39 MBytes       
[  5]   9.00-10.00  sec  1.09 GBytes  9.40 Gbits/sec  370   1.47 MBytes       
- - - - - - - - - - - - - - - - - - - - - - - - -
[ ID] Interval           Transfer     Bitrate         Retr
[  5]   0.00-10.00  sec  10.9 GBytes  9.33 Gbits/sec  11176             sender
[  5]   0.00-10.04  sec  10.9 GBytes  9.29 Gbits/sec                  receiver

iperf Done
```

Les différences constatées sont sans doutes lié à des variations locales.


## Autre test
### IP
#### Standard
Sens 2 :
```
root@DPDK-two:~# iperf3 -c 192.168.129.1
Connecting to host 192.168.129.1, port 5201
[  5] local 192.168.130.1 port 35054 connected to 192.168.129.1 port 5201
[ ID] Interval           Transfer     Bitrate         Retr  Cwnd
[  5]   0.00-1.00   sec  1.08 GBytes  9.31 Gbits/sec  187   1.54 MBytes       
[  5]   1.00-2.00   sec  1.09 GBytes  9.41 Gbits/sec  306   2.00 MBytes       
[  5]   2.00-3.00   sec  1.09 GBytes  9.38 Gbits/sec  520   2.32 MBytes       
[  5]   3.00-4.00   sec  1.09 GBytes  9.38 Gbits/sec  821   1.98 MBytes       
[  5]   4.00-5.00   sec  1.09 GBytes  9.39 Gbits/sec   72   1.80 MBytes       
[  5]   5.00-6.00   sec  1.10 GBytes  9.41 Gbits/sec    0   2.21 MBytes       
[  5]   6.00-7.00   sec  1.09 GBytes  9.41 Gbits/sec    0   2.55 MBytes       
[  5]   7.00-8.00   sec  1.08 GBytes  9.30 Gbits/sec  609   2.80 MBytes       
[  5]   8.00-9.00   sec  1.09 GBytes  9.37 Gbits/sec  266   3.00 MBytes       
[  5]   9.00-10.00  sec  1.09 GBytes  9.39 Gbits/sec    7   3.00 MBytes       
- - - - - - - - - - - - - - - - - - - - - - - - -
[ ID] Interval           Transfer     Bitrate         Retr
[  5]   0.00-10.00  sec  10.9 GBytes  9.37 Gbits/sec  2788             sender
[  5]   0.00-10.04  sec  10.9 GBytes  9.33 Gbits/sec                  receiver

iperf Done.
```
#### MSS
```
root@DPDK-two:~# iperf3 -c 192.168.129.1 --set-mss 100
Connecting to host 192.168.129.1, port 5201
[  5] local 192.168.130.1 port 49426 connected to 192.168.129.1 port 5201
[ ID] Interval           Transfer     Bitrate         Retr  Cwnd
[  5]   0.00-1.00   sec   161 MBytes  1.35 Gbits/sec  126    244 KBytes       
[  5]   1.00-2.00   sec   179 MBytes  1.50 Gbits/sec    0    276 KBytes       
[  5]   2.00-3.00   sec   178 MBytes  1.49 Gbits/sec   30    218 KBytes       
[  5]   3.00-4.00   sec   178 MBytes  1.49 Gbits/sec    0    251 KBytes       
[  5]   4.00-5.00   sec   179 MBytes  1.50 Gbits/sec    0    283 KBytes       
[  5]   5.00-6.00   sec   179 MBytes  1.50 Gbits/sec   36    224 KBytes       
[  5]   6.00-7.00   sec   179 MBytes  1.50 Gbits/sec    0    259 KBytes       
[  5]   7.00-8.00   sec   180 MBytes  1.51 Gbits/sec    0    290 KBytes       
[  5]   8.00-9.00   sec   179 MBytes  1.50 Gbits/sec    4    232 KBytes       
[  5]   9.00-10.00  sec   179 MBytes  1.50 Gbits/sec    0    266 KBytes       
- - - - - - - - - - - - - - - - - - - - - - - - -
[ ID] Interval           Transfer     Bitrate         Retr
[  5]   0.00-10.00  sec  1.73 GBytes  1.48 Gbits/sec  196             sender
[  5]   0.00-10.04  sec  1.72 GBytes  1.48 Gbits/sec                  receiver

iperf Done.
```