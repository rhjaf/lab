# Install XDP 🚀  
[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](https://choosealicense.com/licenses/gpl-3.0/)  


## Commands 
xpd virtion_net Paravirtualized Network interface
```bash
git clone https://github.com/libbpf/libbpf.git
libbpf0 libbpfcc libbpf-dev libxdp libxdp-devel xdp-tools bpftool libc6-dev-i386
cd libbpf/src
make && make install
make install_headers
ldconfig
cd xdp
clang -o2 -g -Wall -target bpf -c main.c -0 main
```
Run the following commands to check the flags:
```bash
sudo ethtool -k enp0s10 | grep rx
sudo ethtool -k enp0s10 | grep sum
sudo ethtool -k enp0s10 | grep tx
sudo ethtool -k enp0s10 | grep gro
```
And for turnining them all down:
```bash
sudo ethtool -K enp0s10 rx tx gro sum off
```
If you want to bind it in Native mode to interface(recommended):
```bash
sudo ip link set dev enp0 xdpdrv object main.o sec xdp_drop
```
Or if your NIC doesn't support it, use the following command to bind it in Generic mode:
```bash
sudo ip link set dev enp0 xdpgeneric object main.o sec xdp_drop
```
And for unbinding the byte code and reset the NIC:
```bash 
sudo ip link set dev enp0 xdp off
```
