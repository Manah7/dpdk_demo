#ifndef DEBUG_DEMO
#define DEBUG_DEMO

#include <rte_ethdev.h>
#include <rte_ether.h>

/* Retourne un (char *) à partir d'une IP RTE */
char * debug_ip(unsigned int ip);

/* Affiche des informations pour un paquet L2 */
void print_debug_mac(struct rte_ether_hdr *mac_hdr, char * type);

/* Affiche des informations pour un paquet L3 */
void print_debug_ip(struct rte_ether_hdr *mac_hdr, 
                    struct rte_ipv4_hdr *ipv4_hdr);

/* Affiche les informations de blocage */
void print_debug_rules(int nb_src_blk, 
                    rte_be32_t *deny_ip_src, 
                    int nb_dst_blk, 
                    rte_be32_t *deny_ip_dst);

#endif
