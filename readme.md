<br />
<p align="center">
  <p align="center">
    <strong>Filtrage haut débit avec DPDK</strong>
    <br />
    <a href="https://github.com/Manah7/dpdk_demo/releases"><strong>Voir les releases »</strong></a>
    <br />
    <br />
        <a href="https://github.com/Manah7/dpdk_demo">
    <img src="img/schema_virt_dpdk.png" alt="Logo" width="500">
    </a>
    <br />
    <a href="rapport.pdf">Rapport</a>
    ·
    <a href="doc/">Documentation</a>
    ·
    <a href="mailto:todo">Contact</a>
  </p>
<br />

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
* Détails des performances sur Proxmox : [doc/perf_dpdk_promox.md](doc/perf_dpdk_promox.md)

### DPDK sur un banc d'essais réel
Résultats : [doc/perf_dpdk_reel.md](doc/perf_dpdk_reel.md).

## Implémentation du module en C
Nous avons implémenté un module DPDK réalisant du filtrage IPv4 simple basé sur l'adresse IP source ou destination d'un paquet. Ce module présente des performances intéressantes que nous essayerons d'évaluer. La configuration du filtrage s'effectue dans un fichier `rules.cfg` dont voici un exemple :
```
# Exemple de fichier de configuration
block src 244.174.221.178

block dst 192.168.0.3
block dst 192.168.0.4
```

### Build
Vous devez avoir installé DPDK (plusieurs exemples dans [/doc](/doc)), vous pouvez build avec :
```bash
make        # Meilleurs performances
make debug  # Avec les informations de debug (lent)
```
Un [script](src/remote_build.sh) est également disponible pour build à distance sur un banc d'essais et/ou si vous n'avez pas d'installation de DPDK en local.

### Lancer le programme
Voir la [/doc](/doc) pour optimiser les performances. Lancer le programme avec :
```bash
./main -l2,3
```

Pour plus d'information, voir le [répertoire src/](src/).

## Informations et contact
Voir [manah.fr](https://manah.fr).

## Licence
Voir [license](license). Nous utilisons la licence du projet DPDK. Des parties de code peuvent être emprunter aux exemples fournis par DPDK (`Copyright(c) Intel Corporation`).
