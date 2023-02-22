#ifndef DEBUG_DEMO
#define DEBUG_DEMO

#include <rte_ethdev.h>
#include <rte_ether.h>

/* Affiche des informations pour un paquet L2 */
void print_debug_mac(struct rte_ether_hdr *mac_hdr);

/* Affiche des informations pour un paquet L3 */
void print_debug_ip(struct rte_ether_hdr *mac_hdr, struct rte_ipv4_hdr *ipv4_hdr);

#endif