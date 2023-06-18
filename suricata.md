# How to Install Suricata  7️⃣.0️⃣.0️⃣💨rc2-dev  🕵️‍♂️
[![License](https://img.shields.io/badge/License-GNU%20GPL-blue)](https://opensource.org/license/gpl-3-0/)

```bash
sudo apt -y install libpcre3 libpcre3-dbg libpcre3-dev libpcre2-dev build-essential autoconf automake libtool libpcap-dev libnet1-dev libyaml-0-2 libyaml-dev pkg-config zlib1g zlib1g-dev libcap-ng-dev libcap-ng0 make libmagic-dev libjansson-dev rustc cargo jq git-core 
```

Next step is downloading DPDK and set it up. here I set to ports for dpdk. change it with your need.
```bash
apt install dpdk dpdk-dev
ifconfig ens160 down
ifconfig ens192 down
modprobe igb
modprobe uio_pci_generic
dpdk-devbind -b uio_pci_generic 0000:03:00.0 
dpdk-devbind -b uio_pci_generic 0000:0b:00.0 
dpdk-hugepages --setup 1G
```

Next, you  need to install cbindgen
```bash
cargo install --force cbindgen
export PATH=$PATH:/root/.cargo/bin
```
Now clone latest version of Suricata in your desired directory
```bash
git clone https://github.com/OISF/suricata.git && cd suricata
```

Next you need to edit enivironment variables. ( replace `/home/lubuntu/` with your current logged in user's home directory)
```bash
echo 'export PATH="${PATH}:/home/lubuntu/.cargo/bin"' >> ~/.bashrc
export PATH="${PATH}:/home/lubuntu/.cargo/bin"
```

Now we will build suricata
```bash
./autogen.sh
./configure --enable-dpdk
make -j 5
```

For running Suricata, in its root directory
```bash
./src/suricata -c suricata.yaml --dpdk -l /tmp/ -S /dev/null -vvv
```
to enable as IPS
```bash
./configure --enable-nfqueue --prefix=/usr --sysconfdir=/etc --localstatedir=/var
```
