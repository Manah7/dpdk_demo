## Performance de DPDK sous Proxmox
### Performance des vSwitchs 
`iperf3` entre dpdk-0 et dpdk-1 :
```
root@dpdk-0:~# iperf3 -c 192.168.0.3
Connecting to host 192.168.0.3, port 5201
[  5] local 192.168.0.1 port 57132 connected to 192.168.0.3 port 5201
[ ID] Interval           Transfer     Bitrate         Retr  Cwnd
[  5]   0.00-1.00   sec  2.44 GBytes  21.0 Gbits/sec    0   3.14 MBytes       
[  5]   1.00-2.00   sec  2.27 GBytes  19.5 Gbits/sec    0   3.14 MBytes       
[  5]   2.00-3.00   sec  2.52 GBytes  21.7 Gbits/sec    0   3.14 MBytes       
[  5]   3.00-4.00   sec  2.49 GBytes  21.3 Gbits/sec    0   3.14 MBytes       
[  5]   4.00-5.00   sec  2.17 GBytes  18.7 Gbits/sec    0   3.14 MBytes       
[  5]   5.00-6.00   sec  2.30 GBytes  19.8 Gbits/sec    0   3.14 MBytes       
[  5]   6.00-7.00   sec  2.40 GBytes  20.6 Gbits/sec    0   3.14 MBytes       
[  5]   7.00-8.00   sec  2.25 GBytes  19.3 Gbits/sec    0   3.14 MBytes       
[  5]   8.00-9.00   sec  2.17 GBytes  18.6 Gbits/sec    0   3.14 MBytes       
[  5]   9.00-10.00  sec  2.28 GBytes  19.6 Gbits/sec    0   3.14 MBytes       
- - - - - - - - - - - - - - - - - - - - - - - - -
[ ID] Interval           Transfer     Bitrate         Retr
[  5]   0.00-10.00  sec  23.3 GBytes  20.0 Gbits/sec    0             sender
[  5]   0.00-10.04  sec  23.3 GBytes  19.9 Gbits/sec                  receiver

iperf Done.
```

### Performance DPDK native
`iperf3` entre dpdk-0 et dpdk-2, avec dpdk-1 relais DPDK en `io packet forwarding` :
```
root@dpdk-0:~# iperf3 -c 192.168.0.2
Connecting to host 192.168.0.2, port 5201
[  5] local 192.168.0.1 port 50344 connected to 192.168.0.2 port 5201
[ ID] Interval           Transfer     Bitrate         Retr  Cwnd
[  5]   0.00-1.00   sec   512 MBytes  4.29 Gbits/sec  227    624 KBytes       
[  5]   1.00-2.00   sec   520 MBytes  4.36 Gbits/sec  702    667 KBytes       
[  5]   2.00-3.00   sec   539 MBytes  4.52 Gbits/sec  800    578 KBytes       
[  5]   3.00-4.00   sec   499 MBytes  4.18 Gbits/sec  1986    428 KBytes       
[  5]   4.00-5.00   sec   585 MBytes  4.91 Gbits/sec  946    303 KBytes       
[  5]   5.00-6.00   sec   534 MBytes  4.48 Gbits/sec  828    276 KBytes       
[  5]   6.00-7.00   sec   555 MBytes  4.66 Gbits/sec  929    279 KBytes       
[  5]   7.00-8.00   sec   510 MBytes  4.28 Gbits/sec  662    540 KBytes       
[  5]   8.00-9.00   sec   496 MBytes  4.16 Gbits/sec  1333    409 KBytes       
[  5]   9.00-10.00  sec   510 MBytes  4.28 Gbits/sec  550    648 KBytes       
- - - - - - - - - - - - - - - - - - - - - - - - -
[ ID] Interval           Transfer     Bitrate         Retr
[  5]   0.00-10.00  sec  5.14 GBytes  4.41 Gbits/sec  8963             sender
[  5]   0.00-10.04  sec  5.13 GBytes  4.39 Gbits/sec                  receiver

iperf Done.
```
Statistiques de `dpdk-testpmd` :
```
testpmd> show port stats all 

  ######################## NIC statistics for port 0  ########################
  RX-packets: 8704614    RX-missed: 0          RX-bytes:  13176718542
  RX-errors: 0
  RX-nombuf:  0         
  TX-packets: 151063     TX-errors: 0          TX-bytes:  10000086

  Throughput (since last show)
  Rx-pps:       392805          Rx-bps:   4755647184
  Tx-pps:         6007          Tx-bps:      3175240
  ############################################################################

  ######################## NIC statistics for port 1  ########################
  RX-packets: 151084     RX-missed: 0          RX-bytes:  10001478
  RX-errors: 0
  RX-nombuf:  0         
  TX-packets: 8684668    TX-errors: 0          TX-bytes:  13146534756

  Throughput (since last show)
  Rx-pps:         6007          Rx-bps:      3175240
  Tx-pps:       392092          Tx-bps:   4747010120
  ############################################################################
  

  ---------------------- Forward statistics for port 0  ----------------------
  RX-packets: 3817326        RX-dropped: 0             RX-total: 3817326
  TX-packets: 64687          TX-dropped: 0             TX-total: 64687
  ----------------------------------------------------------------------------

  ---------------------- Forward statistics for port 1  ----------------------
  RX-packets: 64687          RX-dropped: 0             RX-total: 64687
  TX-packets: 3808363        TX-dropped: 8963          TX-total: 3817326
  ----------------------------------------------------------------------------

  +++++++++++++++ Accumulated forward statistics for all ports+++++++++++++++
  RX-packets: 3882013        RX-dropped: 0             RX-total: 3882013
  TX-packets: 3873050        TX-dropped: 8963          TX-total: 3882013
  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
```


### Performance avec iptables (sans DPDK)
```
root@dpdk-0:~# iperf3 -c 192.168.1.1 
Connecting to host 192.168.1.1, port 5201
[  5] local 192.168.0.1 port 59868 connected to 192.168.1.1 port 5201
[ ID] Interval           Transfer     Bitrate         Retr  Cwnd
[  5]   0.00-1.00   sec  1.19 GBytes  10.2 Gbits/sec    0   3.15 MBytes       
[  5]   1.00-2.00   sec  1.17 GBytes  10.1 Gbits/sec    0   3.15 MBytes       
[  5]   2.00-3.00   sec  1.17 GBytes  10.0 Gbits/sec    0   3.15 MBytes       
[  5]   3.00-4.00   sec  1.10 GBytes  9.43 Gbits/sec    0   3.15 MBytes       
[  5]   4.00-5.00   sec  1.19 GBytes  10.2 Gbits/sec    0   3.15 MBytes       
[  5]   5.00-6.00   sec  1.18 GBytes  10.1 Gbits/sec    0   3.15 MBytes       
[  5]   6.00-7.00   sec  1.11 GBytes  9.52 Gbits/sec    0   3.15 MBytes       
[  5]   7.00-8.00   sec  1.14 GBytes  9.76 Gbits/sec    0   3.15 MBytes       
[  5]   8.00-9.00   sec  1.17 GBytes  10.1 Gbits/sec    0   3.15 MBytes       
[  5]   9.00-10.00  sec  1.16 GBytes  9.94 Gbits/sec    0   3.15 MBytes       
- - - - - - - - - - - - - - - - - - - - - - - - -
[ ID] Interval           Transfer     Bitrate         Retr
[  5]   0.00-10.00  sec  11.6 GBytes  9.94 Gbits/sec    0             sender
[  5]   0.00-10.04  sec  11.6 GBytes  9.90 Gbits/sec                  receiver

iperf Done.
```
Avec la configuration actuelle, `iptables` est plus rapide avec des paquets standards -_-

### Tentative de meilleurs performances avec configuration spécifique
```
## AVEC DPDK
# iperf3 -c 192.168.0.2 M128 -l64 
Connecting to host 192.168.0.2, port 5201
[  5] local 192.168.0.1 port 56706 connected to 192.168.0.2 port 5201
[ ID] Interval           Transfer     Bitrate         Retr  Cwnd
[  5]   0.00-1.00   sec  23.9 MBytes   200 Mbits/sec    0   67.9 KBytes       
[  5]   1.00-2.00   sec  23.6 MBytes   198 Mbits/sec    0   76.4 KBytes       
[  5]   2.00-3.00   sec  24.0 MBytes   202 Mbits/sec    0   80.6 KBytes       
[  5]   3.00-4.00   sec  22.8 MBytes   191 Mbits/sec    0   80.6 KBytes       
[  5]   4.00-5.00   sec  23.1 MBytes   193 Mbits/sec    0   80.6 KBytes       
[  5]   5.00-6.00   sec  24.5 MBytes   206 Mbits/sec    0   80.6 KBytes       
[  5]   6.00-7.00   sec  23.9 MBytes   201 Mbits/sec    0   80.6 KBytes       
[  5]   7.00-8.00   sec  23.1 MBytes   194 Mbits/sec    0   80.6 KBytes       
[  5]   8.00-9.00   sec  23.0 MBytes   193 Mbits/sec    0   80.6 KBytes       
[  5]   9.00-10.00  sec  23.0 MBytes   193 Mbits/sec    0   80.6 KBytes       
- - - - - - - - - - - - - - - - - - - - - - - - -
[ ID] Interval           Transfer     Bitrate         Retr
[  5]   0.00-10.00  sec   235 MBytes   197 Mbits/sec    0             sender
[  5]   0.00-10.04  sec   235 MBytes   196 Mbits/sec                  receiver

iperf Done.

## SANS DPDK :
# iperf3 -c 192.168.1.1 M128 -l64 
Connecting to host 192.168.1.1, port 5201
[  5] local 192.168.0.1 port 37746 connected to 192.168.1.1 port 5201
[ ID] Interval           Transfer     Bitrate         Retr  Cwnd
[  5]   0.00-1.00   sec  25.7 MBytes   215 Mbits/sec    0    273 KBytes       
[  5]   1.00-2.00   sec  24.5 MBytes   206 Mbits/sec    0    273 KBytes       
[  5]   2.00-3.00   sec  24.6 MBytes   206 Mbits/sec    0    273 KBytes       
[  5]   3.00-4.00   sec  24.4 MBytes   205 Mbits/sec    0    273 KBytes       
[  5]   4.00-5.00   sec  24.5 MBytes   205 Mbits/sec    0    273 KBytes       
[  5]   5.00-6.00   sec  24.7 MBytes   207 Mbits/sec    0    273 KBytes       
[  5]   6.00-7.00   sec  24.5 MBytes   205 Mbits/sec    0    273 KBytes       
[  5]   7.00-8.00   sec  24.6 MBytes   207 Mbits/sec    0    273 KBytes       
[  5]   8.00-9.00   sec  24.6 MBytes   206 Mbits/sec    0    273 KBytes       
[  5]   9.00-10.00  sec  24.8 MBytes   208 Mbits/sec    0    273 KBytes       
- - - - - - - - - - - - - - - - - - - - - - - - -
[ ID] Interval           Transfer     Bitrate         Retr
[  5]   0.00-10.00  sec   247 MBytes   207 Mbits/sec    0             sender
[  5]   0.00-10.04  sec   246 MBytes   205 Mbits/sec                  receiver

iperf Done.
```