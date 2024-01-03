# DPDK :boom: Setup
[![License](https://img.shields.io/badge/License-BSD_3--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)


In this guide, I will show you how to setup a Ubuntu Machine that uses DPDK :two::two:.:one::one:.:one:.

## Requirements

Create additional interface so you can connect to internet from your VM and also may be able to SSH to it.
Notice that you should properly change the type of virtual machine NICs that you want to attach DPDK on to `vmxnet3` ( in case if you are using VMware workstation). In VMware for doing that, you should modify `.vmx` file of that machine and change the `.virtualdev` of the designated NIC.eg: `ethernet1.virtualdev = "vmxnet3"`

### platform: Ubuntu 22.04 on VMWare workstation with 6GB RAM, 4 cores, and 5 NICs (Recommended : 1 (management)+even).
## Commands
```bash
sudo apt install net-tools git clang llvm gcc build-essential meson python3-pyelftools libnuma-dev pkgconf dpdk-kmods-dkms
cd opt
wget --no-check-certificate http://fast.dpdk.org/rel/dpdk-22.11.1.tar.xz
tar -xf dpdk-22.11.1.tar.xz
cd dpdk-22.11.1
meson build
ninja -C build
cd build
sudo ninja install
ldconfig
```
Check for compatible vfio (virtio-net)
```bash
dpdk-devbind.py -s
```
Turn the interface down:
```bash
ifconfig enp160 down
```
Check the device again:
```bash
dpdk-devbind.py -s 
```
Loading UIO driver module. we use uio because vfio has some issues inside vms.
```bash
modprobe uio
modprobe igb_uio
```
Check the device again:
```bash
dpdk-devbind.py -s
```
Add module UIO to PCIe device:
```bash
dpdk-devbind.py -b igb_uio 0000:00:08.0
```
Check the device again:
```bash
dpdk-devbind.py -s
```
Now we build a sample `helloworld` example:
```bash
cd dpdk/examples/helloworld
sudo make
sudo su
```
But before running any DPDK, we should set some requirements. Here we should reserve huge pages of memory:
```bash
mkdir -p /dev/hugepages
mountpoint -q /dev/hugepages || mount -t hugetlbfs nodev /dev/hugepages
echo 1024 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
```
Now we can run the application
```bash
cd build
```
As you know, we should setup some EAL parameters, that are necessary. We specify the number of cores to run (lcore 0 and lcore 1) and the number of Memory channels-RAM slots- that it can use as 2. ( if you need info for your device use:`htop` , `meminfo`, `lstopo`)
```bash
sudo ./helloworld -l 0-1 -n 2 
```
for some applications, for example `testpmd`, you may need to set (limit) the size of `mbuf` pool (it should be greater than 1024 and less than 3500):
```bash
sudo dpdk-testpmd -l 0-1 -- --total-num-mbufs=1025
```
Now we can also run l2fwd application on 2 ports. (`-p` is for portmask)(NOTE that you should have generated traffic to these ports! **[TREX](https://github.com/rhjaf/lab/blob/main/TREX-setup.md)**) 
```bash
sudo ./l2fwd -- -p 0x3 -P --no-mac-updating
```

### Some extra notes
If you have one single interface and you use it as ssh but also you want to give it to DPDK (regulary in cloud machines like [this](www.packet.com)), you should SR-IOV on that interface so that it seems to be like multiple PCIs. Add the following to `GRUB_CMDLINE_LINUX` in `/etc/default/grub`:
```bash
iommu=pt intel_iommu=on pci=assign-busses
```
After that run `update-grub && reboot`.
Now we can set the number of $Virtual Functions$ (new virtual PCIs) like this:
```
echo 1 > /sys/class/net/eno1/device/sriov_numvfs
ip link set eno1 vf 0 spoofchk off
ip link set eno1 vf 0 trust on
```
Now type `dmesg -t` to get the MAC on VF. The output should be something like this:
```bash
[Tue Mar 17 19:44:37 2020] i40e 0000:02:00.0: Allocating 1 VFs.
[Tue Mar 17 19:44:37 2020] iommu: Adding device 0000:03:02.0 to group 1
[Tue Mar 17 19:44:38 2020] iavf 0000:03:02.0: Multiqueue Enabled: Queue pair count = 4
[Tue Mar 17 19:44:38 2020] iavf 0000:03:02.0: MAC address: 1a:b5:ea:3e:28:92
[Tue Mar 17 19:44:38 2020] iavf 0000:03:02.0: GRO is enabled
[Tue Mar 17 19:44:39 2020] iavf 0000:03:02.0 em1_0: renamed from eth0
```
Run this to get device name
```bash
lshw -businfo -class network | grep 000:03:02.0
```
Finally, you can unbind it from kernel and add it to DPDK using this:
```bash
dpdk-devbind -b igb_uio 0000:03:02.0
```
- If you want to unbind your NIC and give it back to Kernel. Here I use VMWare as a Hypervisor, so I bind it to `vmxnet3`:
```bash
dpdk-devbind.py -b vmxnet3 03:00.0
```
### Server EFD nodes
Run a server with 5 nodes
```bash
./server/build/server -- -p 0x3 -n 5
```
Run a node which is number 3
```bash
./build/node --proc-type=secondary -- -n 3
```
### Multi process  in DPDK

[GUIDE](https://doc.dpdk.org/guides/prog_guide/multi_proc_support.html)

Each application can have different permission on shared memory and huge pages

- primary process: which can initialize and which have full permissions on shared memory
- secondary process: which cannot initialize shared memory, but can attach to pre- initialized shared memory and create objects in it

Relative parameters:
- `--proc-type`:
- `--file-prefix`:  to allow processes that do not want to co-operate to have different memory regions.

**NOTE**!! Secondary processes which require access to physical devices in the Primary process, must be passed with the same allow and block options.

Deployment models:
- Symmetric/Peer processes: each process performs the same workload like having multiple threads each running the same main loop (like most DPDK sample applications). The first process spwaned using `--proc-type=primary` and the subsequent instances should be spawned by `--proc-type=secondary`. eg: simple_mp, symmetric_mp
- Asymmetric/Non-peer processes: A single primary process acts as load-balancer for distributing packets among worker threads. `rte_ring` is used for that. eg: client_server_mp
- Multiple independent applications: `--file-prefix`. They should explicitly limit their memory use. It is not a problem in linux. but if `--legacy-mem` is used, it can get all memory it get to and should be limited by `-m 3mb` to each process to specify how much hugepage can use. Or by passing `--socket-mem` to specify how much hugepage memory on each socket each process can use. Note that indepent DPDK instances can not share any network ports
- Multiple Independent Groups

#### Shared memory

The EAL puts shared runtime files in /var/run/dpdk (if it is runned by root permission). Hugepage files use `rtemap_x` where x is from 0 to max number of hugpages. It also creates `.rte_config` file for each process to share config files and memory mapped

limitations:
- Disable ASLR to able to map memory
- All same shared memory DPDK processes should have distinict coremask arguments.
- All Ethernet interrupts are triggered in Primary process.
- The use of function pointers between multiple processes running based of different compiled binaries is not supported.
- Depending upon the hardware in use, and the number of DPDK processes used, it may not be possible to have HPET timers available in each DPDK instance
#### Communication between Multiple process
DPDK IPC API is used as a convenient way for communicating via messages between primary and secondary messages. It supports **Unicast**: from secondary to primary and **Broadcast**: from primary to all secondaries. Any IPC message sent in a primary process will be delivered to all secondaries, while any IPC message sent in a secondary process will only be delivered to primary process. Unicast from primary to secondary or from secondary to secondary is not supported. There are three types of communication procedures includes: Message, Synchronous request, Asynchronous request. with which Message doesn't excpet respond (it is not two-way)

### Distributor

[GUIDE](https://doc.dpdk.org/guides/prog_guide/packet_distrib_lib.html)
```bash
./distributor_app -- -p 0x3 -c 0xff
```
