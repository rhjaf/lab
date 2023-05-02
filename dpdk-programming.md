
First of all, we should usually setup these steps:
Setup EAL environment:
```c
/*
   * EAL: "Environment Abstraction Layer"
   * EAL gets parameters from cli, returns number of parsed args
   * cpu_init: fill cpu_info structure
   * log_init
   * config_init: create memory configuration in shared memory
   * pci_init: scan pci bus
   * memory_init (hugepages)
   * memzone_init: initialize memzone subsystem
   * alarm_init: for timer interrupts
   * timer_init
   * plugin init
   * dev_init: initialize and probe virtual devices
   * intr_init: create na interrupt handler thread
   * lcore_init: Create a thread per lcore
   * pci_probe: probel all physical devices
   */
  ret = rte_eal_init(argc,argv);
```
get number of available ports
```c
u_int8_t number_of_ports = rte_eth_dev_count_avail();
```
allocate mempool buffer for
```c
struct rte_mempool *mbuf_pool;
mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL",number_of_mbufs_for_each_port * number_of_ports, MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
```
for allocating buffers from this pool, so you can put receiving `burst` packets inside it
```c
struct rte_mbuf *bufs[BURST_SIZE];
u_int16_t nb_rx = rte_eth_rx_burst(port,0,bufs,BURST_SIZE);
```
and for storing data inside

NOTE!!! you can store ethernet headers inside `rte_ether_hdr` structure.

NOTE!!! second argument is the type of input data that you are casting from it.
```c
rte_pktmbuf_mtod(bufs[i], struct rte_ether_hdr *);
```
After that you can get these memory to ports and initilize them
```c
port_init(portid,mbuf_pool);
```
but remeber for working and configuring Ethernet port and interacting with Ethernet API. you should run this on desired device when it is stopped
```c
int ret = rte_eth_dev_configure(port, nb_rx_queues, nb_tx_queues, &port_conf);
```
NOTE!!! after that you can setup a queue(rx or tx) on a port and allocating membuf pool and finally start the port
```c
rte_eth_rx_queue_setup(....)
rte_eth_dev_start(port)
```
3rd parameter is port configuaration, for example:
```c
struct rte_eth_conf port_conf = {
    .rxmode = { .max_rx_pkt_len = RTE_ETHER_MAX_LEN }
  };
```
logging
```c
rte_exit(EXIT_FAILURE,"Invalid port number\n");
```
```c
RTE_LOG(INFO, APP, "Number of ports:%u\n",number_of_ports);
```
for assigning threads to cores and skipping MAIN_THREAD as a worker thread
```c
rte_eal_mp_remote_launch(worker_main,NULL,SKIP_MAIN);
```
waiting MAIN_THREAD until all SLAVEs are finisihed or interrupted:
```c
rte_eal_mp_wait_lcore();
```
to get id of current logical core
```c
unsigned int lcore_id = rte_lcore_id();
```
you can also get id of specific socket id
```c
rte_eth_dev_socket_id(port);
```
