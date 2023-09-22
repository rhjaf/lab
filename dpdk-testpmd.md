
`dpdk-testpmd` is an application for testing DPDK and using as base ground for developing DPDK applications.
You can see the available options of it by:
```bash
dpdk-testpmd -- --help
```
for running in interactive mode, use `-i` option.
Here I set a scenario for sending packets to a linux via 2 bonded NIC.

```bash
testpmd> create bonded device 2 0
testpmd> add bonding slave 0 2
testpmd> add bonding slave 1 2
testpmd> set bonding mode 4 2
testpmd> set bonding primary 0 2
testpmd> set fwd txonly
testpmd> port start
testpmd> port start 0
testpmd> port start 1
testpmd> port start 2
testpmd> show port info 2
testpmd> set eth-peer  0 xx:xx:xx:xx:xx:xx
testpmd> set eth-peer  1 xx:xx:xx:xx:xx:xx
testpmd> set eth-peer  2 xx:xx:xx:xx:xx:xx
testpmd> start
```
The forwarding modes are: io|mac|macswap|flowgen|rxonly|txonly|csum|icmpecho|noisy|5tswap|shared_rxq [dpdk_forwarding_modes][https://doc.dpdk.org/guides/testpmd_app_ug/testpmd_funcs.html#set-fwd]
