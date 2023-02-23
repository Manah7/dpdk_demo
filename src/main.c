#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_ether.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>

#include "init.h"
#include "debug.h"

#define BURST_SIZE 1024

/* 
    Test de l'environnement de développement. 
    Version modifié d'un exemple DPDK.

    Pour tester sur le banc d'essais, utilisez remote_build.sh.
*/

/* Affiche des informations très verbeuses sur chaque paquet */
#define DEBUG 1


/* From http://www.rfc-editor.org/rfc/rfc1812.txt section 5.2.2 
 * Inutilsé pour le moment */
static inline int is_valid_ipv4_pkt(struct rte_ipv4_hdr *pkt, uint32_t ll)
{
    if (ll < sizeof(struct rte_ipv4_hdr))
        return -1;

    if (((pkt->version_ihl) >> 4) != 4)
        return -3;

    if ((pkt->version_ihl & 0xf) < 5)
        return -4;

    if (rte_cpu_to_be_16(pkt->total_length) < sizeof(struct rte_ipv4_hdr))
        return -5;

    return 0;
}


/* Parse a given config. file */
int parse_config(char * filename){
    // TODO
}


/* Libère la mémoire d'un paquet donné */
void drop(struct rte_mbuf * pkt){
    rte_pktmbuf_free(pkt);
    if (unlikely(DEBUG))
        printf("\t...dropped...\n");
}


/* Boucle principale lcore_main */
static __rte_noreturn void lcore_main(void)
{
    uint16_t port;

    /* Vérification prise d'un exemple : Check that the port is on the same 
     * NUMA node as the polling thread for best performance. */
    RTE_ETH_FOREACH_DEV(port)
        if (rte_eth_dev_socket_id(port) >= 0 &&
                rte_eth_dev_socket_id(port) != (int)rte_socket_id())
            printf("[!] Port %u : conf. NUMA invalide.\n", port);

    if (DEBUG) {printf("DEBUG mode: will very (very) verbose\n");}
    printf("\nCore %u forwarding packets [Ctrl+C to quit]\n", rte_lcore_id());

    /* Main work of application loop.*/
    for (;;) {
        RTE_ETH_FOREACH_DEV(port) {
            struct rte_mbuf *bufs_tx[BURST_SIZE];
            struct rte_mbuf *bufs_rx[BURST_SIZE];

            /* Get burst of RX packets, from first port of pair. */
            uint16_t nb_rt = 0;
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
                if (eth_hdr->ether_type == rte_cpu_to_be_16(RTE_ETHER_TYPE_ARP)){
                    if (unlikely(DEBUG)) {print_debug_mac(eth_hdr, "ARP");}

                    /* On garde le paquet */
                    bufs_tx[nb_rt] = bufs_rx[pkt_id];
                    nb_rt++;
                }
                /* IPv4 */
                else if (eth_hdr->ether_type == rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4)){
                    struct rte_ipv4_hdr *ipv4_hdr = 
                    rte_pktmbuf_mtod_offset(bufs_rx[pkt_id], 
                    struct rte_ipv4_hdr *, 
                    sizeof(struct rte_ether_hdr));

                    /* On garde le paquet */
                    bufs_tx[nb_rt] = bufs_rx[pkt_id];
                    nb_rt++;

                     if (unlikely(DEBUG)) {print_debug_ip(eth_hdr, ipv4_hdr);}
                } 
                /* IPv6 */
                else if (eth_hdr->ether_type == rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV6)){
                    if (unlikely(DEBUG)) {print_debug_mac(eth_hdr, "IP6");}
                    drop(bufs_rx[pkt_id]);
                }
                /* Non identifié */
                else {
                    if (unlikely(DEBUG)) {print_debug_mac(eth_hdr, "UKN");}
                    drop(bufs_rx[pkt_id]);
                }
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
    /* Initialise l'EAL et RTE */
    rte_init(argc, argv);

    /* Parse config file */
    parse_config("rules.cfg"); // TODO

    /* Boucle principale, sur un coeur. Un seul appel depuis le main. */
    lcore_main();

    /* Clean up the EAL */
    rte_eal_cleanup();

    return 0;
}
