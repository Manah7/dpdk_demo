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
#define DEBUG 0


/* From http://www.rfc-editor.org/rfc/rfc1812.txt section 5.2.2 */
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

    if (DEBUG)
        printf("DEBUG mode: will very (very) verbose\n");
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
                struct rte_ether_hdr *mac_hdr = 
                    rte_pktmbuf_mtod(bufs_rx[pkt_id], 
                    struct rte_ether_hdr *);
                
                /* Header IP */ 
                struct rte_ipv4_hdr *ipv4_hdr = 
                    rte_pktmbuf_mtod_offset(bufs_rx[pkt_id], 
                    struct rte_ipv4_hdr *, 
                    sizeof(struct rte_ether_hdr));

                /* Paquet IP valide */
                if (unlikely(DEBUG) && is_valid_ipv4_pkt(ipv4_hdr, 
                        bufs_rx[pkt_id]->pkt_len) < 0)
                    if (unlikely(DEBUG)) {print_debug_mac(mac_hdr);}
                else
                    if (unlikely(DEBUG)) {print_debug_ip(mac_hdr, ipv4_hdr);}

                /* On regarde si on garde le paquet ou non. */
                if (1){
                    bufs_tx[nb_rt] = bufs_rx[pkt_id];
                    nb_rt++;
                } else {
                    if (unlikely(DEBUG))
                        printf("\t...dropped...\n");
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

            // TODO 
            // Free bufs_rx
        }
    }
}


/* Point d'entrée, init. & args. */
int main(int argc, char *argv[]){
    /* Initialise l'EAL et RTE */
    rte_init(argc, argv);

    /* Boucle principale, sur un coeur. Un seul appel depuis le main. */
    lcore_main();

    /* Clean up the EAL */
    rte_eal_cleanup();

    return 0;
}
