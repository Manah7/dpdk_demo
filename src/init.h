#ifndef INIT_DEMO
#define INIT_DEMO

#include <rte_ethdev.h>

#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250

/* Récupéré d'un exemple : initialise un port donné. */
int port_init(uint16_t port, struct rte_mempool *mbuf_pool);

/* Récupéré d'un exemple : Initialise l'EAL et parse les arguments */
int rte_init(int * argc, char ** argv[]);

#endif
