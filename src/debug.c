#include "debug.h"

/*
 * Fonctions d'affichage des informations de debug
 */
char * debug_ip(unsigned int ip)
{
    unsigned char bytes[4];
    char * s_ip = malloc(4*3+4);

    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;   
    sprintf(s_ip, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]); 

    return s_ip; 
}
char * debug_mac(struct rte_ether_addr addr){
    char * s_mac = malloc(6*2+6);
    sprintf(s_mac, "%02X:%02X:%02X:%02X:%02X:%02X", 
        addr.addr_bytes[0],
        addr.addr_bytes[1],
        addr.addr_bytes[2],
        addr.addr_bytes[3],
        addr.addr_bytes[4],
        addr.addr_bytes[5]);
    return s_mac;
}

void print_debug_mac(struct rte_ether_hdr *mac_hdr){
    printf(" [%lu] Paquet UKN : %s --> %s\n",
        (unsigned long)time(NULL),
        debug_mac(mac_hdr->src_addr), 
        debug_mac(mac_hdr->dst_addr));
}

void print_debug_ip(struct rte_ether_hdr *mac_hdr, struct rte_ipv4_hdr *ipv4_hdr){
    printf(" [%lu] Paquet IPv4 : %s --> %s (%s --> %s)\n",
        (unsigned long)time(NULL), 
        debug_ip(ipv4_hdr->src_addr),
        debug_ip(ipv4_hdr->dst_addr),
        debug_mac(mac_hdr->src_addr), 
        debug_mac(mac_hdr->dst_addr));
}
