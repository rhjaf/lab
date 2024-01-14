# PF_RING

### Setup PFRing Intel driver:
1- Insert PF_RING Module
```bash
sudo insmod pf_ring.ko min_num_slots=65536 enable_tx_capture=0
```
2- Load PF_RING driver for you Intel NIC:
```bash
cd PF_RING/drivers/intel
./configure && make
cd ixgbe/ixgbe-*-zc/src
sudo ./load_driver.sh
```
3- Run PF_RING with DPDK example:
```bash
cd /opt/PF_RING/userland/examples_ft/
./ftflow_dpdk -- -v -7 -n 3 -p 0x2 (you can use -F option too)
```
