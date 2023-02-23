#ifndef PARSE_DEMO
#define PARSE_DEMO

#include <rte_ether.h>
#include <rte_ethdev.h>

/* Parse a given config. file (doc -> cf. parse.c) */
int parse_config(char * filename,  int *nb_src_blk, rte_be32_t **deny_ip_src, int *nb_dst_blk, rte_be32_t **deny_ip_dst, int DEBUG);

#endif
