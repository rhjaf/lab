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
dpdk-devbind.py -b uio_pci_generic 0000:03:00.0 
dpdk-devbind.py -b uio_pci_generic 0000:0b:00.0 
dpdk-hugepages.py --setup 1G
```
Remember to check huge-page's allocation by `dpdk-hugepages.py --show` and rerun the allocation again if needed.

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
./bundle.sh
./autogen.sh
./configure --enable-dpdk
make -j4
```

If you want to contribute to Suricata, build with following options
```bash
CFLAGS="-O0 -ggdb -fno-omit-frame-pointer" ./configure --disable-shared --enable-debug --enable-unittests --enable-dpdk
```
Next you need to edit desired dpdk interface in `suricata.yaml` config. also set `set-cpu-affinity` to `yes`. also you can determine worker CPU's ids in `cpu` field of `worker-cpu-set`.

For running Suricata, in its root directory (using DPDK capture):
```bash
./src/suricata -c suricata.yaml --dpdk -l /tmp/ -S /dev/null -vvv
```
Here we set the working rules to nothing. You can define your rules directory which will be used by default when running Suricata, in `suricata.yaml`. And also you can add the more roles you want with `-s` option. (`-S` will ignore the default rules directory specified in config file).

### IPS mode
Be able to receive traffic and return them afterward, either from interface to interface directly with AF_PACKET or within netfilter (kernel tcp/ip) stack via NFQUEUE where you can use routing etc. click here [ℹ️](https://docs.suricata.io/en/suricata-6.0.0/setting-up-ipsinline-for-linux.html))
- NFQUEUE
```bash
./configure --enable-nfqueue --prefix=/usr --sysconfdir=/etc --localstatedir=/var
```
  but maybe you need this following command
```bash
sudo apt-get -y install libnetfilter-queue-dev libnetfilter-queue1 libnfnetlink-dev libnfnetlink0
```
  and also you may consider netfilter configurations (check the documentation)
- DPDK

  add  `copy-mode: ips` and you can also use `copy-iface: correspond_interface name` to forward traffic on another interface (like DPDK l2fwd). (Note that if you want to use this, the `threads` value for each interface should be same. for example set them to `1`). then run:
```bash
./src/suricata -c suricata.yaml --dpdk -l /tmp/ -S /dev/null -vvv
```
For more information on configuring Suricata and other things, you can visit this site 'https://jufajardini.wordpress.com/' or use Suricon videos available on YouTube.
You can also use 'https://github.com/StamusNetworks/suricata-language-server' for Suricata code highlighting
