## DPDK avec vhost-user
Lien du tuto original : [https://www.redhat.com/en/blog/hands-vhost-user-warm-welcome-dpdk](https://www.redhat.com/en/blog/hands-vhost-user-warm-welcome-dpdk)

Le tuto est largement adapté pour pouvoir utiliser Ubuntu 20.04 et prendre en compte les restrictions de paquet apparues depuis la plublication du tuto.

### Création de la VM
```bash
sudo wget -O /var/lib/libvirt/images/CentOS-7-x86_64-GenericCloud.qcow2 http://cloud.centos.org/centos/7/images/CentOS-7-x86_64-GenericCloud.qcow2

sudo qemu-img create -f qcow2 -b  /var/lib/libvirt/images/CentOS-7-x86_64-GenericCloud.qcow2   /var/lib/libvirt/images//vhuser-test1.qcow2 20G

export LIBVIRT_DEFAULT_URI="qemu:///system"

sudo virt-sysprep --root-password password:test1 --uninstall cloud-init --selinux-relabel -a /var/lib/libvirt/images/vhuser-test1.qcow2 --network --install “dpdk,dpdk-tools,pciutils”
```

Si KVM/qemu était déjà installé, il n'est pas nécessaire de créer le réseau par défault, on vérifie juste qu'il est là :
```bash
virsh net-list
```

Création de la VM :
```bash
virt-install --import  --name vhuser-test1 --ram=4096 --vcpus=3 \
--nographics --accelerate \
       --network network:default,model=virtio --mac 02:ca:fe:fa:ce:aa \
      --debug --wait 0 --console pty \
      --disk /var/lib/libvirt/images/vhuser-test1.qcow2,bus=virtio --os-variant centos7.0

virsh list
virsh shutdown vhuser-test1
```

### Préparation de l'hôte (Ubuntu 20.04)
On modifie la cmdline du noyau Linux dans GRUB :
```bash
sudo nano /etc/default/grub
sudo update-grub
reboot
```
en ajoutant `default_hugepagesz=1G hugepagesz=1G hugepages=8 iommu=pt intel_iommu=on` dans `GRUB_CMDLINE_LINUX_DEFAULT`. On vérifie avec :
```bash
cat /proc/cmdline
grep Huge /proc/meminfo
```

On modifie aussi la configuration de qemu avec `user = "root"` dans `/etc/libvirt/qemu.conf`.

### Modification de la VM
```bash
virsh edit vhuser-test1
```
Configuration complète obtenue, pour les instructions spécifique, voir le tutoriel original.
```xml
<domain type='kvm'>
  <name>vhuser-test1</name>
  <uuid>bd77bbc5-de11-43d0-8f4b-8dbd248a88cc</uuid>
  <metadata>
    <libosinfo:libosinfo xmlns:libosinfo="http://libosinfo.org/xmlns/libvirt/domain/1.0">
      <libosinfo:os id="http://centos.org/centos/7.0"/>
    </libosinfo:libosinfo>
  </metadata>
  <memory unit='KiB'>3145728</memory>
  <currentMemory unit='KiB'>3145728</currentMemory>
  <memoryBacking>
    <hugepages>
      <page size='1048576' unit='KiB' nodeset='0'/>
    </hugepages>
    <locked/>
  </memoryBacking>
  <vcpu placement='static'>3</vcpu>
  <numatune>
    <memory mode='strict' nodeset='0'/>
  </numatune>
  <os>
    <type arch='x86_64' machine='pc-q35-4.2'>hvm</type>
    <boot dev='hd'/>
  </os>
  <features>
    <acpi/>
    <apic/>
  </features>
  <cpu mode='host-passthrough' check='none'>
    <topology sockets='1' cores='3' threads='1'/>
    <numa>
      <cell id='0' cpus='0-2' memory='3145728' unit='KiB' memAccess='shared'/>
    </numa>
  </cpu>
  <clock offset='utc'>
    <timer name='rtc' tickpolicy='catchup'/>
    <timer name='pit' tickpolicy='delay'/>
    <timer name='hpet' present='no'/>
  </clock>
  <on_poweroff>destroy</on_poweroff>
  <on_reboot>restart</on_reboot>
  <on_crash>destroy</on_crash>
  <pm>
    <suspend-to-mem enabled='no'/>
    <suspend-to-disk enabled='no'/>
  </pm>
  <devices>
    <emulator>/usr/bin/qemu-system-x86_64</emulator>
    <disk type='file' device='disk'>
      <driver name='qemu' type='qcow2'/>
      <source file='/var/lib/libvirt/images/vhuser-test1.qcow2'/>
      <target dev='vda' bus='virtio'/>
      <address type='pci' domain='0x0000' bus='0x04' slot='0x00' function='0x0'/>
    </disk>
    <controller type='usb' index='0' model='qemu-xhci' ports='15'>
      <address type='pci' domain='0x0000' bus='0x02' slot='0x00' function='0x0'/>
    </controller>
    <controller type='sata' index='0'>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x1f' function='0x2'/>
    </controller>
    <controller type='pci' index='0' model='pcie-root'/>
    <controller type='pci' index='1' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='1' port='0x8'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x01' function='0x0' multifunction='on'/>
    </controller>
    <controller type='pci' index='2' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='2' port='0x9'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x01' function='0x1'/>
    </controller>
    <controller type='pci' index='3' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='3' port='0xa'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x01' function='0x2'/>
    </controller>
    <controller type='pci' index='4' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='4' port='0xb'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x01' function='0x3'/>
    </controller>
    <controller type='pci' index='5' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='5' port='0xc'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x01' function='0x4'/>
    </controller>
    <controller type='pci' index='6' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='6' port='0xd'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x01' function='0x5'/>
    </controller>
    <controller type='pci' index='7' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='7' port='0xe'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x01' function='0x6'/>
    </controller>
    <controller type='pci' index='8' model='pcie-root-port'>
      <model name='pcie-root-port'/>
      <target chassis='8' port='0xf'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x01' function='0x7'/>
    </controller>
    <controller type='virtio-serial' index='0'>
      <address type='pci' domain='0x0000' bus='0x03' slot='0x00' function='0x0'/>
    </controller>
    <interface type='network'>
      <mac address='02:ca:fe:fa:ce:aa'/>
      <source network='default'/>
      <model type='virtio'/>
      <address type='pci' domain='0x0000' bus='0x01' slot='0x00' function='0x0'/>
    </interface>

    <interface type='vhostuser'>
      <mac address='56:48:4f:53:54:01'/>
      <source type='unix' path='/tmp/vhost-user1' mode='client'/>
      <model type='virtio'/>
      <driver name='vhost' rx_queue_size='256'/>
      <address type='pci' domain='0x0000' bus='0x07' slot='0x00' function='0x0'/>
    </interface>
    <interface type='vhostuser'>
      <mac address='56:48:4f:53:54:02'/>
      <source type='unix' path='/tmp/vhost-user2' mode='client'/>
      <model type='virtio'/>
      <driver name='vhost' rx_queue_size='256'/>
      <address type='pci' domain='0x0000' bus='0x08' slot='0x00' function='0x0'/>
    </interface>

    <serial type='pty'>
      <target type='isa-serial' port='0'>
        <model name='isa-serial'/>
      </target>
    </serial>
    <console type='pty'>
      <target type='serial' port='0'/>
    </console>
    <channel type='unix'>
      <target type='virtio' name='org.qemu.guest_agent.0'/>
      <address type='virtio-serial' controller='0' bus='0' port='1'/>
    </channel>
    <input type='mouse' bus='ps2'/>
    <input type='keyboard' bus='ps2'/>
    <memballoon model='virtio'>
      <address type='pci' domain='0x0000' bus='0x05' slot='0x00' function='0x0'/>
    </memballoon>
    <rng model='virtio'>
      <backend model='random'>/dev/urandom</backend>
      <address type='pci' domain='0x0000' bus='0x06' slot='0x00' function='0x0'/>
    </rng>
  </devices>
</domain>
```

### Lancement de la VM et des tests
Dans un premier terminal :
```bash
sudo dpdk-testpmd -l 0,2,3,4,5 --socket-mem=1024 -n 4 \
    --vdev 'net_vhost0,iface=/tmp/vhost-user1' \
    --vdev 'net_vhost1,iface=/tmp/vhost-user2' -- \
    --portmask=f -i --rxq=1 --txq=1 \
    --nb-cores=4 --forward-mode=io
```
Et dans un second :
```bash
virsh start vhuser-test1
virsh console vhuser-test1
```

#### Finalisation de la configuration du guest
**Sur la machine guest,** on fait les dernières préparations :
```bash
modprobe vfio enable_unsafe_noiommu_mode=1
modprobe vfio-pci
/usr/share/dpdk/usertools/dpdk-devbind.py -b vfio-pci 0000:07:00.0 0000:08:00.0
dpdk-devbind --status net
```
**Attention**, les modifications ne sont pas persistantes

#### Génération du trafic
Génération du trafic, setup hôte éteint.

**Sur l'hôte :**
```bash
sudo dpdk-testpmd -l 0,2,3,4,5 --socket-mem=1024 -n 4 \
    --vdev 'net_vhost0,iface=/tmp/vhost-user1' \
    --vdev 'net_vhost1,iface=/tmp/vhost-user2' -- \
    --portmask=f -i --rxq=1 --txq=1 \
    --nb-cores=4 --forward-mode=io
```

```bash
virsh start vhuser-test1
virsh console vhuser-test1
```

**Sur le guest :**
```bash
# Ajout de hugepages sur le guest
echo 800 > /proc/sys/vm/nr_hugepages
echo 1 > /sys/module/vfio/parameters/enable_unsafe_noiommu_mode

# Modprobe
modprobe vfio enable_unsafe_noiommu_mode=1
modprobe vfio-pci

# Driver DPDK
dpdk-devbind -b vfio-pci 0000:07:00.0 0000:08:00.0

# Lancement du test
testpmd -l 0,1,2 --socket-mem 1024 -n 4 \
    --proc-type auto --file-prefix pg -- \
    --portmask=3 --forward-mode=macswap --port-topology=chained \
    --disable-rss -i --rxq=1 --txq=1 \
    --rxd=256 --txd=256 --nb-cores=2 --auto-start
```
Un script permet d'automatiser cela : [script_vm.sh](script_vm.sh)


#### Résultats
Sur le guest :
```
start tx_first

show port stats all
show port info all

stop
```
Exemple de résultats :
```
testpmd> show port stats all

  ######################## NIC statistics for port 0  ########################
  RX-packets: 64691648   RX-missed: 0          RX-bytes:  4140265472
  RX-errors: 0
  RX-nombuf:  0         
  TX-packets: 64571328   TX-errors: 0          TX-bytes:  4132564992

  Throughput (since last show)
  Rx-pps:      3727350          Rx-bps:   1908403440
  Tx-pps:      3713164          Tx-bps:   1901140352
  ############################################################################

  ######################## NIC statistics for port 1  ########################
  RX-packets: 64571520   RX-missed: 0          RX-bytes:  4132577280
  RX-errors: 0
  RX-nombuf:  0         
  TX-packets: 64691904   TX-errors: 0          TX-bytes:  4140281856

  Throughput (since last show)
  Rx-pps:      3713168          Rx-bps:   1901142216
  Tx-pps:      3727373          Tx-bps:   1908415296
  ############################################################################


Telling cores to stop...
Waiting for lcores to finish...

  ---------------------- Forward statistics for port 0  ----------------------
  RX-packets: 12572480       RX-dropped: 0             RX-total: 12572480
  TX-packets: 12721184       TX-dropped: 0             TX-total: 12721184
  ----------------------------------------------------------------------------

  ---------------------- Forward statistics for port 1  ----------------------
  RX-packets: 12721152       RX-dropped: 0             RX-total: 12721152
  TX-packets: 12572512       TX-dropped: 0             TX-total: 12572512
  ----------------------------------------------------------------------------

  +++++++++++++++ Accumulated forward statistics for all ports+++++++++++++++
  RX-packets: 25293632       RX-dropped: 0             RX-total: 25293632
  TX-packets: 25293696       TX-dropped: 0             TX-total: 25293696
  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

```

