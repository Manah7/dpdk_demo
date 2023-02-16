# Projet long 
Ce répertoire contient quelques tutos et configurations établies lors de notre projet long de Master SSIR. Ce répertoire n'est pas exhaustif du travail fournis, mais donne un aperçu des résultats obtenus. Pour plus de détails sur notre méthodologie et nos réalisations, merci de consulter le rapport du projet *(accès restreint)*.

## Configurations réalisées
### Introduction à DPDK
Une petite introduction pour s'assurer que DPDK est installé et fonctionne :
* DPDK basics : [doc/basic_dpdk.md](doc/basic_dpdk.md)

### DPDK avec KVM 
Dans cette section, nous détaillons fortement les étapes suivies, notamment pour la configuration de DPDK sur un VM locale tournant sous KVM. Le but est de testé un peu plus les performances et de réussir à utiliser un exemple de code C.
* DPDK KVM avec `testpmd` : [doc/kvm_dpdk.md](doc/kvm_dpdk.md)
* DPDK KVM avec `l2fwd` : [doc/kvm_l2fwd.md](doc/kvm_l2fwd.md)

### DPDK dans une infrastructure virtuelle
Dans cette section, nous présentons la mise en place de DPDK sur un banc d'essai virtuel :
* Banc d'essais sur hyperviseur : [doc/hyperviseur_dpdk.md](doc/hyperviseur_dpdk.md)
* Détails des performances sur Proxmox (*désuet*) : [doc/perf_dpdk_promox.md](doc/perf_dpdk_promox.md)

## Implémentation du module en C
Voir le [répertoire src/](src/).

## Informations et contact
Voir [manah.fr](https://manah.fr).

## Licence
Voir [license](license). Nous utilisons la licence du projet DPDK. Des parties de code peuvent être emprunter aux exemples fournis par DPDK (`Copyright(c) Intel Corporation`).
