## Générateur et analyse des performances
Le rapport du projet fournis une explication plus détaillée de cette partie, il s'agit ici d'une annexe. 

Il est difficile de trouver un générateur de flux suffisament efficace pour tester DPDK. Il s'agit le plus souvent d'outil utilisant eux-même DPDK, souvent assez compliqué à configurer. Nous allons ici parcourir les différentes solutions que nous avons envisagées et/ou utilisées.

### Iperf3
Il s'agit d'un utilitaire de test grand public et fonctionne d'end-point à end-point. Sa facilité d'utilisation (ligne de commande, options simples) a fait que nous l'avons privilégié lors de notre projet. Néanmoins, il utilise la stack Linux standard, ce qui fait qu'il ne permet pas de tester un module DPDK qui serait plus efficace que cette dernière. Nous avons donc cherché dans la suite du projet d'autre utilitaire détaillées après.

### Pkg-gen / Moonpol
Basé sur libmoon, ne compile pas, documentation dépréciée. Meilleur avancement jusqu'à maintenant, build partiel en copiant les fichiers à la places des examples :
```
# cd pktgen-dpdk/
# make
>>> Use 'make help' for more commands

./tools/pktgen-build.sh build
>>  SDK Path          : /root/test/dpdk/
>>  Install Path      : /root/test/dpdk/examples/pktgen-dpdk
>>  Build Directory   : /root/test/dpdk/examples/pktgen-dpdk/Builddir
>>  Target Directory  : usr/local
>>  Build Path        : /root/test/dpdk/examples/pktgen-dpdk/Builddir
>>  Target Path       : /root/test/dpdk/examples/pktgen-dpdk/usr/local

 Build and install values:
   lua_enabled       : -Denable_lua=false
   gui_enabled       : -Denable_gui=false

>>> Ninja build in '/root/test/dpdk/examples/pktgen-dpdk/Builddir' buildtype=release
meson -Dbuildtype=release -Denable_lua=false -Denable_gui=false /root/test/dpdk/examples/pktgen-dpdk/Builddir
The Meson build system
Version: 0.56.2
Source dir: /root/test/dpdk/examples/pktgen-dpdk
Build dir: /root/test/dpdk/examples/pktgen-dpdk/Builddir
Build type: native build
Program cat found: YES (/usr/bin/cat)
Project name: pktgen
Project version: 22.04.1
C compiler for the host machine: cc (gcc 10.2.1 "cc (Debian 10.2.1-6) 10.2.1 20210110")
C linker for the host machine: cc ld.bfd 2.35.2
Host machine cpu family: x86_64
Host machine cpu: x86_64
Compiler for C supports arguments -mavx: YES 
Compiler for C supports arguments -mavx2: YES 
Compiler for C supports arguments -Wno-pedantic: YES 
Compiler for C supports arguments -Wno-format-truncation: YES 
Found pkg-config: /usr/bin/pkg-config (0.29.2)
Run-time dependency libdpdk found: YES 20.11.6
Library librte_net_bond found: YES
Program python3 found: YES (/usr/bin/python3)
Library rte_net_i40e found: YES
Library rte_net_ixgbe found: YES
Library rte_net_ice found: YES
Library rte_bus_vdev found: YES
Run-time dependency threads found: YES
Library numa found: YES
Library pcap found: YES
Library dl found: YES
Library m found: YES
Program sphinx-build found: NO
Build targets in project: 9

Found ninja-1.10.1 at /usr/bin/ninja
ninja: Entering directory `/root/test/dpdk/examples/pktgen-dpdk/Builddir'
[40/69] Compiling C object app/pktgen.p/pktgen-cfg.c.o
FAILED: app/pktgen.p/pktgen-cfg.c.o 
cc -Iapp/pktgen.p -Iapp -I../app -Ilib/common -I../lib/common -Ilib/utils -I../lib/utils -Ilib/vec -I../lib/vec -Ilib/plugin -I../lib/plugin -Ilib/cli -I../lib/cli -Ilib/lua -I../lib/lua -I/usr/include/dpdk -I/usr/include/libnl3 -I/usr/include/dpdk/../x86_64-linux-gnu/dpdk -fdiagnostics-color=always -pipe -D_FILE_OFFSET_BITS=64 -Wall -Winvalid-pch -Wextra -Wpedantic -Werror -O3 -march=native -mavx -mavx2 -DALLOW_EXPERIMENTAL_API -D_GNU_SOURCE -Wno-pedantic -Wno-format-truncation -pthread -include rte_config.h -march=corei7 '-D__PROJECT_VERSION="22.04.1"' -MD -MQ app/pktgen.p/pktgen-cfg.c.o -MF app/pktgen.p/pktgen-cfg.c.o.d -o app/pktgen.p/pktgen-cfg.c.o -c ../app/pktgen-cfg.c
In file included from ../app/pktgen.h:88,
                 from ../app/pktgen-display.h:17,
                 from ../app/pktgen-cfg.c:11:
../app/pktgen-port-cfg.h: In function ‘rte_get_rx_capa_list’:
../app/pktgen-port-cfg.h:416:22: error: ‘RTE_ETH_RX_OFFLOAD_VLAN_STRIP’ undeclared (first use in this function); did you mean ‘DEV_RX_OFFLOAD_VLAN_STRIP’?
  416 |     } rx_flags[] = {{RTE_ETH_RX_OFFLOAD_VLAN_STRIP, _(VLAN_STRIP)},
      |                      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                      DEV_RX_OFFLOAD_VLAN_STRIP

            ...
```

### TRex (Cisco)
Installation et build ok. Configuration compiquée et kernel panic dans certains cas. [Documentation bien faite et complète.](https://trex-tgn.cisco.com/trex/doc/trex_manual.html) Installation :
```bash
mkdir -p /opt/trex && cd /opt/trex
wget --no-cache --no-check-certificate https://trex-tgn.cisco.com/trex/release/latest
tar -xzvf latest && rm latest

cd v[VERSION]

./dpdk_setup_ports.py -s # Montre les ports disponibles
./dpdk_setup_ports.py -i # Génération intéractive de fichier de conf.
```

Config. utilisée (générée automatiquement) :
```
- version: 2
  interfaces: ['0000:04:00.0', '0000:04:00.1']
  port_info:
      - ip: 1.1.1.1
        default_gw: 2.2.2.2
      - ip: 2.2.2.2
        default_gw: 1.1.1.1

  platform:
      master_thread_id: 0
      latency_thread_id: 11
      dual_if:
        - socket: 0
          threads: [1,2,3,4,5,6,7,8,9,10]
```

**Erreur au lancement :**
```
# ./t-rex-64 -f cap2/dns.yaml -c 4 -m 1 -d 10
Trying to bind to vfio-pci ...
/usr/bin/python3 dpdk_nic_bind.py --bind=vfio-pci 0000:04:00.0 0000:04:00.1 
The ports are bound/configured.
Starting  TRex v3.02 please wait  ... 
 set driver name net_ixgbe 
 driver capability  : TCP_UDP_OFFLOAD  TSO  SLRO 
 set dpdk queues mode to DROP_QUE_FILTER 
 Number of ports found: 2
zmq publisher at: tcp://*:4500
ixgbe_dev_start(): failure in ixgbe_dev_start(): -38
EAL: Error disabling MSI-X interrupts for fd 35
ixgbe_dev_start(): failure in ixgbe_dev_start(): -38
EAL: Error disabling MSI-X interrupts for fd 35
ixgbe_dev_start(): failure in ixgbe_dev_start(): -38
EAL: Error disabling MSI-X interrupts for fd 35
ixgbe_dev_start(): failure in ixgbe_dev_start(): -38
EAL: Error disabling MSI-X interrupts for fd 35
ixgbe_dev_start(): failure in ixgbe_dev_start(): -38
EAL: Error disabling MSI-X interrupts for fd 35
ixgbe_dev_start(): failure in ixgbe_dev_start(): -38
EAL: Error disabling MSI-X interrupts for fd 35
ixgbe_dev_start(): failure in ixgbe_dev_start(): -38
EAL: Error disabling MSI-X interrupts for fd 35
ixgbe_dev_start(): failure in ixgbe_dev_start(): -38
EAL: Error disabling MSI-X interrupts for fd 35
ixgbe_dev_start(): failure in ixgbe_dev_start(): -38
EAL: Error disabling MSI-X interrupts for fd 35
ixgbe_dev_start(): failure in ixgbe_dev_start(): -38
EAL: Error - exiting with code: 1
  Cause: rte_eth_dev_start: err=-5, port=0
```
Semble similaire à cette erreur, pas d esolution : https://github.com/cisco-system-traffic-generator/trex-core/issues/76


