# Install XDP 🚀  
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)  


## Commands 

Check to see if XDP is enabled in the kernel. XDP has been supported by kernel>4.8
```bash
grep -i CONFIG_XDP_SOCKETS /boot/config-$(uname -r)
```
It is also important that your NIC also should have support XDP. unless you can run it only in generic mode

Reduce MTU to which XDP can supports 
```bash
sudo ip link set dev ens37 mtu 3818
```
Create a file named main.c and :
```c
include <linux/bpf.h>
#define SEC(NAME) __attribute__((section(NAME), used))
SEC("prog")
int  xdp_prog_simple(struct xdp_md *ctx)
{
return XDP_DROP;
}
char _license[] SEC("license") = "GPL";
```
For building XDP object file you need clang compiler
```bash
clang -O2 -g -Wall -target bpf -c main.c -o main.o
```
If you couldn't build the file , try running the below commands
```bash
sudo apt install pkg-config m4 libelf-dev libpcap-dev gcc-multilib
```
For binding it to NIC in Native mode which is recommended
```bash
sudo ip link set dev ens37 xdpdrv object main.o sec prog
```
If your NIC doesn't support it, use the following command to bind it in Generic mode:
```bash
sudo ip link set dev  ens37 xdpgeneric object main.o sec xdp_drop
```
And for unbinding the byte code and reset the NIC:
```bash 
sudo ip link set dev ens37 xdp off
```
