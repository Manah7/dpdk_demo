## Performances de DPDK sur un banc d'essais réel
La forme du banc d'essais est la même que pour la partie virtualisée. Plusieurs parametres ont été ajusté dans la situation réel, notamment au niveau du noyau Linux :
```
GRUB_CMDLINE_LINUX="iommu=pt intel_iommu=on default_hugepagesz=1G hugepagesz=1G hugepages=16 isolcpus=2,3 nohz_full=2,3 rcu_nocbs=2,3"
```
C'est choix ont été fait à l'aide du [benchmark intel](https://fast.dpdk.org/doc/perf/DPDK_22_11_Intel_NIC_performance_report.pdf), mais ne semblent pas améliorer significativement les performances.

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

Les différences constatées sont sans doutes lié à des variations locales.
