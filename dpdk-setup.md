# DPDK :boom: Setup
[![License](https://img.shields.io/badge/License-BSD_3--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)


In this guide I will show you how to setup a Ubuntu Machine that uses DPDK :two::two:.:one::one:.:one:.

## Requirements

Create additional interface so you can connect to internet from your VM and also may be able to SSH to it.

### platform : Ubuntu 22.04 on VMWare workstation with 6GB RAM, 4 cores,and 5 NICs (Recommended : 1+even).
## Commands
```bash
sudo apt install net-tools git clang llvm gcc  kerenl-headers build-essential meson ninja python3-pyelftools libnuma-dev pkgconf
cd opt
wget --no-check-certificate http://fast.dpdk.org/rel/dpdk-22.11.1.tar.xz
tar -xf dpdk-22.11.1.tar.xz
cd dpdk
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
ifconfig enps down
```
Check the device agagin:
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
But before running any DPDK, we should set some requirements. Here we should reserve huge pages memory:
```bash
mkdir -p /dev/hugepages
mountpoint -q /dev/hugepages || mount -t hugetlbfs nodev /dev/hugepages
echo 64 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
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
for running `l2fwd` example on 2 ports, first you should probably change the virtual machine NIC type to `vmxnet3`. In VMware for doing that, you should modify `.vmx` file of that machine and change the `.virtualdev` of the designated NIC.eg: `ethernet1.virtualdev = "vmxnet3"`
Now run application on 2 ports. (`-p` is for portmask)(NOTE that you should have generated traffic to these ports! **[TREX](https://github.com/rhjaf/lab/blob/main/TREX-setup.md)**) 
```bash
sudo ./l2fwd -- -p 0x3
```
