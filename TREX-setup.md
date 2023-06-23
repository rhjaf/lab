# Trex 🦖 Setup

In this guide I will show you how to setup a traffic generator Centos Machine that uses latest version of Trex.

### platform : Centos 7 on VMWare workstation with 4GB RAM, 2 cores,and 3 NICs (Recommended : 1 (for management) + even).


Notice that because Trex uses DPDK, your network driver should also support PMD driver. for that you should modify `.vmx` file of that machine and change the `.virtualdev` of the designated NIC.eg: `ethernet1.virtualdev = "vmxnet3"`

Download Trex
```bash
wget --no-check-certificate https://trex-tgn.cisco.com/trex/release/latest
```

Then you should configure the ports you wonna use for Trex
```bash
sudo ./dpdk_setup_ports.py -c 02:05.0 02:06.0
```
Some considarations:
 - We can also use interface's name instead of MAC.
 - Low end laptio. we can use threads assigned to specific cores.
 - Your system should have at least 3 cores for 2 interfaces, and it has: 2

After doing all configs, you can run basic TRex for 10 seconds, using the following command and it will use the default config file in `/etc/trex_cfg.yaml`:
```bash
sudo ./t-rex-64 -f cap2/dns.yaml --cfg /etc/trex_cfg.yaml -c 4 -m 1 -d 10
```
