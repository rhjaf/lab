
`dpdk-testpmd` is an application for testing DPDK and using as base ground for developing DPDK applications. It is an application for configuring ethernet devices and displaying info about them.
You can see the available options of it by:
```bash
dpdk-testpmd -- --help
```
You can run it in interactive mode by `-i` option. and then execute commands :

Also you can run it with preconfig for ports:
```bash
dpdk-testpmd -c 0xf -n 4  -- -i --port-topology=chained --txq=16 --rxq=16
```

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
The forwarding modes are: io|mac|macswap|flowgen|rxonly|txonly|csum|icmpecho|noisy|5tswap|shared_rxq [dpdk_forwarding_modes](https://doc.dpdk.org/guides/testpmd_app_ug/testpmd_funcs.html#set-fwd)_

### Queue and port configurations
```bash
testpmd> show rxq info port# queue#
testpmd> port port# rxq rxq# stop|setup|start
```
show info about specific ports
```bash
show port info 1
```
set the rx queues for all of available ports
```bash
port config all rxd 2
```
