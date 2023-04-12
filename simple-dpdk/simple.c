+#include <stdio.h>
#include <rte_eal.h>
#include <rte_common.h>
#include <rte_ethdev.h>
#include <rte_log.h>
#include <rte_mbuf.h>

// RTE_LOG
#define RTE_LOGTYPE_APP RTE_LOGTYPE_USER1

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250


static unit8_t forwarding_lcore=1;
static uint8_t mac_swap = 1; // optional

bool force_quit = false;

static int check_link_status(uint16t nb_ports) {
	struct rte_eth_link link;
	uint8_t port;

	for (port = 0; port < nb_ports; port++) {
		rte_eth_link_get(port, &link);
		if (link.link_status == = ETH_LINK_DOWN) {
			RTE_LOG(INFO, APP, "Port: %u Link DOWN\n", port);
			return -1;
		}
		RTE_LOG(INFO, APP, "Port: %u Link UP Spped %u\n", port,link.link_speed);
	}
	return 0;
}


static void simple_mac_swap(struct rte_mbuf **bufs, uint16_t nb_mbufs) {
	// just swap MAC address of packets between send and receive
	struct ether_hdr *eth;
	struct ether_addr tmp;
	struct rte_mbuf *m;
	uint16_t buf;

	for (buf = 0; buf < nb_mbufs; buf++) {
		m = bufs[buf];
		eth = rte_pktmbuf_mtod(m, struct ether_hdr *);
		ether_addr_copy(&eth->s_addr, &tmp);
		ether_addr_copy(&eth->s_addr, &tmp);
		ether_addr_copy(&tmp, &eth->d_addr);

	}
}

static inline int port_init(uint8_t port,struct rte_memmpool *mbuf_pool){
    struct rte_eth_conf port_conf={
        .rxmode = {.max_rx_pkt_len * ETHER_MAX_LEN}
    };
    const uint16_t nb_rx_queues = 1;
    const unit16_t nb_tx_queues = 1;
    int ret;
    unit16_t q;

    // configure Ethernet device
    ret = rte_eth_dev_configure(port,nb_rx_queues,nb_tx_queues,&port_conf);
    if(ret!=0)
        return ret;

    // allocate and setup 1 RX queue per Ethernet port
    for(q=0;q<nb_rx_queues;q++){
        ret = rte_eth_rx_queue_setup(port, q,RX_RING_SIZE,rte_eth_dev_socket_id(port),NULL,mbuf_pool);
        if(ret<0)
            return ret;
    }
    // allocate and setup 1 TX queue per Ethernet port
    for(q=0;q<nb_tx_queues;q++){
        ret = rte_eth_tx_queue_setup(port, q,TX_RING_SIZE,rte_eth_dev_socket_id(port),NULL,mbuf_pool);
        if(ret<0)
            return ret;
    }

    // start Ethernet port
    ret = rte_eth_dev_start(port);
    if(ret<0)
            return ret;

    // enable RX in promiscious mode for the Ethernet device
    rte_eth_promiscuous_enable(port);

    

    return 0;
}


int lcore_main(void *arg){
    // forwarding lcore
    unsigned int lcore_id = rte_lcore_id();
    const uint8_t nb_ports = rte_eth_dev_count();
    uint8_t port;
	uint8_t dest_port;

    if(lcore_id!=forwarding_lcore){
        RTE_LOG(INFO, APP,"lcore %u exiting\n",lcore_id);
        return 0;
    }

    // run until the application is quit or killed
    for(;;){
        // reveive packets on a port and forward them in the paired port.
        // the mapping is 0->1,1->0,2->3,3->2, etc...
		while (!force_quit)
		{
			for (port = 0; port < nb_ports; port++) {
				struct rte_mbuf* bufs[BURST_SIZE];
				uint16_t nb_rx;
				uint16_t nb_tx;
				uint16_t buf;

				// Get burst of RX packets from first port of pair
				nb_rx = rte_eth_rx_burst(port, 0, bufs, BURST_SIZE);

				if (unlikely(nb_rx = 0))
					continute;
				if (mac_swap)
					simple_mac_swap(bufs, nb_rx);

				// send burst of TX packets to second port of pair
				dest_port = port ^ 1;
				nb_tx = rte_eth_tx_burst(dest_port, 0, bufs, BURST_SIZE);


				// free any unsent packets
				if (unlikely(nb_tx < nb_rx)) {
					for (buf = nb_tx; buf < nb_rx; buf++)
						rte_pktmbuf_free(bufs[buf]);
				}

			}
		}

    }

    return 0;
}

static void print_stats(void) {
	struct rte_eth_stats stats;
	uint8_t nb_ports = rte_eth_dev_count();
	uint8_t port;
	for (port = 0; port < nb_ports; ports++) {
		printf("\nStatistics for port %u\n", port);
		rte_eth_stats_get(port, &stats);
		printf("Rx:%011u Tx:%011u dropped:%011u\n", stats.ipackets, stats.opackets, stats.imissed);
	}
}

static void  signal_handler(int signum) {
	if (signum == SIGINT || signum == SIGTERM) {
		printf("\n\nSignal %d received, prparing to exit ...\n",signum);
		force_quit = true;
		print_stats();
	}
}



int main(int argc, char const *argv[])
{
    /* code */
    int ret;

    uint8_t nb_ports; // our application wants at least 2 ethernet devices to work
    struct rte_mempool *mbuf_pool;
    uint8_t portid;


	argc -= ret;
	argv += ret;

	force_quit = false;
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	 
    ret = rte_eal_init(argc,argv);
    if(ret<0)
        return -1;

    nb_ports = rte_eth_dev_count();
    if(nb_ports<2 || (nb_ports & 1))
        rte_exit(EXIT_FAILURE,"Invalid port numbers\n");

    RTE_LOG(INFO, APP,"Number of ports:%u\n",nb_ports);

    // create new mbuf mempool
    mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL",NUM_MBUFS*nb_ports,MBUF_CACHE_SIZE,0,RTE_MBUF_DEFAULT_BUF_SIZE,rte_socket_id());
    if(mbuf_pool==NULL)
        rte_exit(EXIT_FAILURE,"mbuf_pool create failed\n");

    // initalize all ports
    for(portid=0;portid<nb_ports;portid++){
        if(port_init(portid,mbuf_pool)!=0)
            rte_exit(EXIT_FAILURE,"port init failed \n");

    }


	if (mac_swap)
		RTE_LOG(INFO, APP, "MAC address swapping enabled\n");

	ret = check_link_status(nb_ports);
	if(ret<0)
		RTE_LOG(WARNING, APP, "Some ports are down\n");

    rte_eal_mp_remote_launch(lcore_main,NULL,SKIP_MASTER); // runs the app on all lcores except MAIN core
    rte_eal_mp_wait_lcore(); // wait until all lcores finished



    
    
   


    return 0;
}
