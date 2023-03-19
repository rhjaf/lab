##virtual-box
create additional interface so you can connect to internet from your VM and also may be able to SSH to it.
Ubuntu 22.04,2GB RAM, 2 core,
sudo apt install net-tools git clang llvm gcc  kerenl-headers build-essential meson ninja python3-pyelftools libnuma-dev pkgconf
cd opt
git clone https://github.com/DPDK/dpdk.git
cd dpdk
meson build
ninja -C build
cd build
sudo ninja install
ldconfig
dpdk-devbind.py -s : check for compatible vfio (virtio-net)
ifconfig enps down : turn it down
dpdk-devbind.py -s : check again
modprobe uio : we use uio because vfio has some issues inside vms.
modprobe uio_pci_generic
dpd-devbind.py -s: check devices again
dpdk-devbind.py -b uio_pci_generic 0000:00:08.0 : add module uio to PCIe device
dpd-devbind.py -s: check for last time
cd dpdk/examples/helloworld
sudo make
sudo su
Reserve huge pages memory:
mkdir -p /dev/hugepages
mountpoint -q /dev/hugepages || mount -t hugetlbfs nodev /dev/hugepages
echo 64 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
cd build
sudo ./helloworld -l 0-1 -n 2 : you can use htop command to see how many cores have you available. -n 2 : if you have two RAM slot
--------------------------------------------
xdp
xpd virtion_net Paravirtualized Network interface
git clone https://github.com/libbpf/libbpf.git
libbpf0 libbpfcc libbpf-dev libxdp libxdp-devel xdp-tools bpftool libc6-dev-i386
cd libbpf/src
make && make install
make install_headers
ldconfig
cd xdp
clang -o2 -g -Wall -target bpf -c main.c -0 main
sudo ethtool -k enp0s10 | grep rx or sum or tx or gro
sudo ethtool -K enp0s10 rx tx gro sum off
sudo ip link set dev enp0 xdpgeneric object main.o sec xdp_drop
or
sudo ip link set dev enp0 xdpdrv object main.o sec xdp_drop
sudo ip link set dev enp0 xdp off
