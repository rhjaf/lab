# Trex 🦖 Setup

In this guide, I will show you how to setup a traffic generator Centos Machine that uses the latest version of Trex.

### platform: Centos 7 on VMWare workstation with 4GB RAM, 3 cores (at least 3 cores for 2 interfaces), and 3 NICs (Recommended: 1 (for management) + even).


Notice that because Trex uses DPDK, your network driver should also support PMD driver. for that you should modify `.vmx` file of that machine and change the `.virtualdev` of the designated NIC.eg: `ethernet1.virtualdev = "vmxnet3"`

Download Trex
```bash
wget --no-check-certificate https://trex-tgn.cisco.com/trex/release/latest
tar -xf latest
```
You need install latest version of python3
```bash
yum install python3
```
You should get the PCI address of two NICs
```bash
./dpdk_nic_bing.py --status
```
Then you should configure the ports you wanna use for Trex
```bash
./dpdk_setup_ports.py -c 0000:03:00.0 0000:0b:00.0
```
After that you should run this command and select the two NICs and then enter yes to other questions
```bash
./dpdk_setup_ports.py -i
```
**Here we want to run trex as backpool, which means the two NICs should be in the same network (connected to the same switch)**.

After doing all configs, you can run basic TRex for 10 seconds, using the following command and it will use the default config file in `/etc/trex_cfg.yaml`:
```bash
sudo ./t-rex-64 -f cap2/dns.yaml --cfg /etc/trex_cfg.yaml  -m 1 -d 10
```
Finally, take some considerations:
 - We can also use the interface's name instead of MAC.
 - Low-end laptop. we can use threads assigned to specific cores.

### TRex Stateless

./t-rexx-64 --stl
./t-rex-64 --stl --dump-interfaces: setup trex in stateless mode and shows its interfaces
./t-rex-64 -i: run trex server in background in another tab
====================================
another_tab:
./trex-console: 



/home/trex/Desktop/pcaps/SAT-01-12-2018_0750

/stl-sim -f stl/pcap.py --yaml


https://trex-tgn.cisco.com/trex/doc/trex_console.html

