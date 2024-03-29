
## First steps:
### Setup EAL environment:
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
allocate mbuf mempool (contains a set of mbuf objects used for storing packets) buffer for each NUMA socket. Every socket has a local memory:
```c
struct rte_mempool *mbuf_pool;
mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL",number_of_mbufs_for_each_port * number_of_ports, MEMPOOL_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
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
After that, you can get these memory to ports and initialize them
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
3rd parameter is port configuaration.

After NIC setup (configs, queue setup), we need to test the NIC by the peer NIC which should be powered on: 
```c
struct rte_eth_link link;
rte_eth_link_get(0,&link); // 0 is port number
```
logging
DPDK logging API:
```c
rte_log(log_level, log_priority)
```
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
burst packets, it can also be used for TX: ( default number of 32 is prefered for many scenarios)
```c
static inline uint16_t rte_eth_rx_burst(uint8_t port_id,uint16_t queue_id, struct rte_mbuf **rx_pkts, const uint16_t nb_pkts);
```
To send packets queued up for transmission on a port and queue  and returns the number of packets successfully sent to the Ethernet device. The error callback is called for any packets which could not be sent. These packets previously buffered via the rte_eth_tx_buffer() function.
```c
int rte_eth_dev_configure	(	uint16_t 	port_id,
uint16_t 	nb_rx_queue,
uint16_t 	nb_tx_queue,
const struct rte_eth_conf * 	eth_conf 
)	
```
To buffer a single packet for future transmission on a port and queue

```c
rte_eth_tx_buffer(uint16_t 	port_id,
uint16_t 	queue_id,
struct rte_eth_dev_tx_buffer * 	buffer,
struct rte_mbuf * 	tx_pkt )
```


In some cases if you need to write address (IP or MAC), consider the following example:
```C
struct rte_flow_item_eth outer_eth_spec = {
  .dst.addr_bytes = "\x00\x11\x22\x33\x44\x66",
};
```
The address `\x00\x11\x22\x33\x44\x66` is equal to `00:11:22:33:44:66`.

Each application can have its own specific CLI arguments (separated by '--' from EAL arguments).

### NIC offloads (FDIR,RSS)
#### Update ( below methods are superseded by the generic flow API (rte_flow) in PMDs that implement the latter. 
for enabling FDIR (Flow Director) on Intel Ethernet ( This traffic from 10.23.4.6 to 10.23.4.18 be placed in queue 4 ):
```bash
ethtool --show-features ens33 | grep ntuple
ethtool --feature ens33 ntuple on
ethtool --config-ntuple flow-type tcp4 src-ip 10.23.4.6 dst-ip 10.23.4.18 action 4
```
for enabling RSS and FDIR on a NIC ( hybrid mode):
- First initialize the NIC ( the regular steps to set up the nic and assign it the queues have not been show):
```c
static struct rte_eth_conf port_conf = {
    .rxmode = {
        .mq_mode = ETH_MQ_RX_RSS,
    },
    .rx_adv_conf = {
        .rss_conf = {
            .rss_key = NULL,
            .rss_hf = ETH_RSS_IP | ETH_RSS_UDP
            ETH_RSS_TCP | ETH_RSS_SCTP,
        },
    }, //Enable RSS
    fdir_conf = {;
        .mode = RTE_FDIR_MODE_PERFECT,
        .pballoc = RTE_FDIR_PBALLOC_64K,
        .status = RTE_FDIR_REPORT_STATUS,
        .mask = {
            .VLAN_tci_mask = 0x0,
            .ipv4_mask = {
                .src_ip = 0xFFFFFFFF,
                .dst_ip = 0xFFFFFFFF,
            },
            .ipv6_mask = {
                .src_ip = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
                .dst_ip = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
            },
            .src_port_mask = 0xFFFF,
            .dst_port_mask = 0xFFFF,
            .mac_addr_byte_mask = 0xFF,
            .tunnel_type_mask = 1,
            .tunnel_id_mask = 0xFFFFFFFF,
        },
        .drop_queue = 127,
    },
};
```
- Next configure FDir to your desire:
```c
struct rte_eth_fdir_filter arg =
{
    .soft_id = 1,
    .input = {
        .flow_type = RTE_ETH_FLOW_NONFRAG_IPV4_UDP,
        .flow = {
            .udp4_flow = {
                    .ip = {
                        .src_ip = 0x03020202, 
                        .dst_ip = 0x05020202,
                    }
                    .src_port = rte_cpu_to_be_16(1024),
                    .dst_port = rte_cpu_to_be_16(1024),
            }
        }
    }
    .action = {
        .rx_queue = 4,
        .behavior= RTE_ETH_FDIR_ACCEPT,
        .report_status = RTE_ETH_FDIR_REPORT_ID,
    }
}
```
- Finally, add the FDir entry to the NIC table:
```c
rte_eth_dev_filter_ctrl(port, RTE_ETH_FILTER_FDIR, RTE_ETH_FILTER_ADD, &arg);
```

### CPU Affinity
```bash
taskset -pc 2839 
```
You can also isolate CPUs in kernel boot parameters
```bash
/etc/default/grub GRUB_CMDLINE_LINUX_DEFAULT="isolcpus=1,2,3,5-7"
```
### Hugpages and Memory
DPDK will use all huge pages, to prevent it use `--socket-mem` option.
You can see and set the number of hugepages in the directory like this: `/sys/devices/system/node/node0/hugepages/hugepages-1048576kB/nr_hugepages`.

Remember to always free buffer storage of packets that have been transmitted. If they have failed, it becomes more important.
### Drivers and IO
Linux provides a mechanism called `UIO` (User space I/O) for users to create programs that directly control hardware. For example, registers that normally control hardware are placed in memory space that is not accessible to the user, but by using the `UIO` mechanism, they are mapped to user-space memory and can be controlled by a user-created program. DPDK realizes faster packet processing by directly controlling the NIC using `UIO`. 

DPDK provides a dedicated device driver for high-speed packet processing for individual NICs using UIO, and the driver is called `PMD` (Poll Mode Driver).
Software-based `PMD`s are also available ==> 
- PCAP-PMD: is not a UIO driver, but a virtual driver that allows DPDK to handle kernel NIC driver. This configuration allows not only packet transmission and reception from the NIC but also packet capture and packet transmission from captured data. You can enable it in the process of DPDK compilation. `CONFIG_RTE_LIBRTE_PMD_PCAP=y` in `dpdk-version/config/common_base` file. for example, if you want to use it in the **l2fwd** example, the process of memory and CPU allocation is the same as before but since the DPDK driver is not allocated this time, it is necessary to specify which NIC is treated as DPDK port: `/build/l2fwd -c 0x3 --vdev=net_pcap0,iface=eth0 --vdev=net_pcap1,iface=eth1 -- -p `

The NIC has a memory area to store received and transmitted packets. This memory area is called a **queue**. Normally, the kernel will dequeue packets received by the NIC or queue packets that it wants to send. But in DPDK, the packet itself is transferred directly from the NIC's queue to the user space (**Hugepage**)

### DPDK as shared library
Do you want to convert DPDK into a shared library?

### Compile DPDK
Makefile: If you want to compile a DPDK application, include `rte.extapp.mk`, but if you want to create a library using DPDK, include `rte.extlib.mk`.

Finally, You can check this blog for more info: https://ibrahimshahzad.github.io/blog
