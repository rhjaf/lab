I assume that you had installed TRex before!
```bash
yum install cmake
```
If you want to change or disable any DPDK feature, you can set the to `n` in the DPDK config files. for example for $numa aware$, you can use this file:
```bash
libmoon/deps/dpdk/config/common_linuxapp
```
Before starting any application and also first time installation ( The installation commands can be found at the repo page), You need to turn off desired interfaces. And then running the installation or the setup-ports shell file.
