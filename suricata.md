```bash
sudo apt-get -y install libpcre3 libpcre3-dbg libpcre3-dev build-essential autoconf automake libtool libpcap-dev libnet1-dev libyaml-0-2 libyaml-dev pkg-config zlib1g zlib1g-dev libcap-ng-dev libcap-ng0 make libmagic-dev libjansson-dev rustc cargo jq git-core
```
Next, you  need to install cbindgen:
```bash
cargo install --force cbindgen
export PATH=$PATH:/root/.cargo/bin
```

```bash
git clone git@github.com:OISF/suricata.git
```
change to download suricata version directory.

```bash
git clone https://github.com/OISF/libhtp.git
./autogen.sh
```

You can change installation directory(check man page)
```bash
./configure --enable-dpdk
```
to enable as IPS
```bash
./configure --enable-nfqueue --prefix=/usr --sysconfdir=/etc --localstatedir=/var
```

```bash
make -j 10
make install
ldconfig
```
