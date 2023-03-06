#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <execinfo.h>

#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_ether.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>

#include "init.h"
#include "debug.h"
#include "parse.h"

#define BURST_SIZE 1024

/* 
 * Module DPDK effectuant un filtrage élémentaire à partir d'un fichier de
 * configuration (rules.cfg).
 * 
 * Pour tester sur le banc d'essais virtuel, utilisez remote_build.sh.
*/


/* Libère la mémoire d'un paquet donné */
inline static int drop(struct rte_mbuf * pkt){
    rte_pktmbuf_free(pkt);
    #ifdef DEBUG
    if (unlikely(DEBUG)) {printf("\t...dropped...\n");}
    #endif
    return 1;
}


/* Boucle principale lcore_main */
static __rte_noreturn void lcore_main(
        int nb_src_blk,
        rte_be32_t *deny_ip_src,
        int nb_dst_blk,
        rte_be32_t *deny_ip_dst)
{
    /* Information sur le coeur de proc. utilisé. */
    printf("Core %u forwarding packets [Ctrl+C to quit]\n", rte_lcore_id());

    /* Boucle de travail. */
    uint16_t port;
    uint16_t nb_rt;
    for (;;) {
        RTE_ETH_FOREACH_DEV(port) {
            struct rte_mbuf *bufs_tx[BURST_SIZE]; // Paquets émits (non filtrés)
            struct rte_mbuf *bufs_rx[BURST_SIZE]; // Paquets reçus

            /* Get burst of RX packets, from first port of pair. */
            nb_rt = 0;
            const uint16_t nb_rx = rte_eth_rx_burst(port, 0,
                    bufs_rx, BURST_SIZE);

            if (unlikely(nb_rx == 0))
                continue;

            /* Boucle sur les paquets du burst */
            for (int pkt_id = 0; pkt_id < nb_rx; ++pkt_id){
                /* Header MAC */
                struct rte_ether_hdr *eth_hdr = 
                    rte_pktmbuf_mtod(bufs_rx[pkt_id], 
                    struct rte_ether_hdr *);

                /* Identification du type de paquet basé sur l'header */
                /* ARP */
                if (unlikely(eth_hdr->ether_type 
                        == rte_cpu_to_be_16(RTE_ETHER_TYPE_ARP))){
                    #ifdef DEBUG
                    if (unlikely(DEBUG)) {print_debug_mac(eth_hdr, "ARP");}
                    #endif

                    /* On garde le paquet */
                    bufs_tx[nb_rt] = bufs_rx[pkt_id];
                    nb_rt++;
                }
                /* IPv4 */
                else if (likely(eth_hdr->ether_type 
                        == rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4))){
                    
                    /* Paquet IPv4 */
                    struct rte_ipv4_hdr *ipv4_hdr = 
                        rte_pktmbuf_mtod_offset(bufs_rx[pkt_id], 
                        struct rte_ipv4_hdr *, 
                        sizeof(struct rte_ether_hdr));

                    #ifdef DEBUG
                    if (unlikely(DEBUG)) {print_debug_ip(eth_hdr, ipv4_hdr);}
                    #endif

                    /* Filtrage du paquet IPv4 en fonction des tables. */
                    for (int i = 0; i < nb_src_blk; i++){
                        if (unlikely(ipv4_hdr->src_addr == deny_ip_src[i])){
                            drop(bufs_rx[pkt_id]);
                            goto pkt_done; /* Sortie */
                        }
                    }

                    for (int i = 0; i < nb_dst_blk; i++){
                        if (unlikely(ipv4_hdr->dst_addr == deny_ip_dst[i])){
                            drop(bufs_rx[pkt_id]);
                            goto pkt_done; /* Sortie */
                        }
                    }

                    /* On garde le paquet si non droppé */
                    bufs_tx[nb_rt] = bufs_rx[pkt_id];
                    nb_rt++;
                    
                } 
                /* IPv6 */
                else if (unlikely(eth_hdr->ether_type 
                        == rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV6))){
                    #ifdef DEBUG
                    if (unlikely(DEBUG)) {print_debug_mac(eth_hdr, "IP6");}
                    #endif
                    drop(bufs_rx[pkt_id]);
                }
                /* Non géré - Voir rte_ether.h:291 */
                else {
                    #ifdef DEBUG
                    if (unlikely(DEBUG)) {print_debug_mac(eth_hdr, "UKN");}
                    #endif
                    drop(bufs_rx[pkt_id]);
                }

                /* Marqueur de sortie en cas de paquet drop. */
                pkt_done:
                ;
            } /* Fin boucle sur les paquets du burst */

            /* Send burst of TX packets, to second port of pair. */
            const uint16_t nb_tx = rte_eth_tx_burst(port ^ 1, 0,
                    bufs_tx, nb_rt);

            /* Free any unsent packets and used memory. */
            if (unlikely(nb_tx < nb_rt)) {
                for (uint16_t buf = nb_tx; buf < nb_rt; ++buf)
                    rte_pktmbuf_free(bufs_tx[buf]);
            }
        }
    }
}


/* Point d'entrée, init. & args. */
int main(int argc, char *argv[]){
    #ifdef DEBUG
    /* On prévient de mode DEBUG : forte reduction des performances. */
    if (DEBUG) {printf("DEBUG mode: will very (very) verbose\n\n");}
    #endif

    /* Initialise l'EAL et RTE */
    rte_init(&argc, &argv);

    /* Gestion des arguments locaux */
    char * filename = "../rules.cfg";       // Fichier par défaut
    if (argc == 1){
        printf("Pas de fichier de configuration donné, utilisation de %s\n",
            filename);
    } else if (argc == 2){
        filename = argv[1];
        printf("Un fichier de configuration donné : %s\n",
            filename);
    } else {
        printf("\nNombre invalide d'argument. Un argument attendu.\n");
        return -2;
    }

    /* Parse config file */
    int nb_src_blk = 0;                     // Nombre de règles blk src
    int nb_dst_blk = 0;                     // Nombre de règles blk dst
    rte_be32_t *deny_ip_src, *deny_ip_dst;  // Table de blocage

    #ifdef DEBUG
    if (parse_config(filename, 
            &nb_src_blk, 
            &deny_ip_src, 
            &nb_dst_blk, 
            &deny_ip_dst, 
            DEBUG) < 0)
    #else
    if (parse_config(filename, 
            &nb_src_blk, 
            &deny_ip_src, 
            &nb_dst_blk, 
            &deny_ip_dst, 
            0) < 0)
    #endif
    {
        printf("\nUnable to read config. file \"%s\".\n", filename);
        return -1;
    }

    /* Affichage des règles utilisées. */
    printf("Règles lues depuis le fichier %s :\n", filename);
    print_debug_rules(nb_src_blk, deny_ip_src, nb_dst_blk, deny_ip_dst);

    /* Boucle principale, sur un coeur. Un seul appel depuis le main. */
    lcore_main(nb_src_blk, deny_ip_src, nb_dst_blk, deny_ip_dst);

    /* Clean up the EAL */
    rte_eal_cleanup();

    return 0;
}
