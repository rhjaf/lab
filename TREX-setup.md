# Trex 🦖 Setup

In this guide, I will show you how to setup a traffic generator Centos Machine that uses the latest version of Trex.

### platform: Centos 7 on VMWare workstation with 4GB RAM, 3 cores (at least 3 cores for 2 interfaces), and 3 NICs (Recommended: 1 (for management) + even).


Notice that because Trex uses DPDK, your network driver should also support PMD driver. for that you should modify `.vmx` file of that machine and change the `.virtualdev` of the designated NIC.eg: `ethernet1.virtualdev = "vmxnet3"`

Download last version  of TReX
```bash
wget --no-check-certificate https://trex-tgn.cisco.com/trex/release/latest
tar -xf latest
```
Other releases with older DPDK versions, may be better. Also you may not be able to use Python API and Stateless GUI with new versions. Run the command `wget --no-cache https://trex-tgn.cisco.com/trex/release/vX.XX.tar.gz` to get other versions, where X.XX is major.minor [look at](https://trex-tgn.cisco.com/trex/doc/release_notes.html).

You need install latest version of python3
```bash
yum install python3
```
You should get the PCI address of two NICs
```bash
./dpdk_nic_bind.py --status
./dpdk_setup_ports.py -t
```
Then you should configure the ports you wanna use for Trex
```bash
./dpdk_setup_ports.py -c 0000:03:00.0 0000:0b:00.0
```
After that you should run this command and select the two NICs and then enter yes to other questions
```bash
./dpdk_setup_ports.py -i
```
**Here we want to run trex as backpool, which means the two NICs should be in the same network (connected to the same switch)**.

After doing all configs, you can run basic TRex for 10 seconds, using the following command and it will use the default config file in `/etc/trex_cfg.yaml`:
```bash
sudo ./t-rex-64 -f cap2/dns.yaml --cfg /etc/trex_cfg.yaml  -m 1 -d 10
```
Finally, take some considerations:
 - We can also use the interface's name instead of MAC.
 - Low-end laptop. we can use threads assigned to specific cores.
 - Each interface MAC if has not defined in `trex_cfg`, will be set hardware default
### TRex Stateless
Start trex server with default `/etc/trex_cfg.yaml` config file (change it with `-c` option)
```bash
./t-rexx-64 --stl
```
Setup trex in stateless mode and shows its interfaces:
```bash
./t-rex-64 --stl --dump-interfaces
```
Run trex server in the background in another tab (You can also use `-c 6` to specify cores):
```bash
./t-rex-64 -i
```
Now for connecting to the server, in another tab of same machine:
- connect with console (Python API):
  For remote connection use `-s`
```bash
./trex-console
```
Sample profile:
```python
from trex_stl_lib.api import *
import argparse


class STLS1(object):

    def create_stream (self):
        return STLStream( 
            packet = 
                    STLPktBuilder(
                        pkt = Ether()/IP(src="16.0.0.1",dst="48.0.0.1")/
                                UDP(dport=12,sport=1025)/(10*'x')
                    ),
             mode = STLTXCont())

    def get_streams (self, tunables, **kwargs):
        parser = argparse.ArgumentParser(description='Argparser for {}'.format(os.path.basename(__file__)), 
                                         formatter_class=argparse.ArgumentDefaultsHelpFormatter)
        args = parser.parse_args(tunables)
        # create 1 stream 
        return [ self.create_stream() ]


# dynamic load - used for trex console or simulator
def register():
    return STLS1()
```
```
trex> start -f stl/udp_1pkt_simple.py -m 10mbps -a ( # you can use -m 100% or --pin or -t size=64,vm=cached)
another example:
trex> start -f stl/bench.py -t size=128,vm=cached --port 0 -m 100% --

trex> streams -a
trex> pause -a, resume -a, stop -a
trex> tui : dynamic stats
trex> portattr -a --prom on  : set promiscuous mode
trex> stats --ps : port status
```
- GUI connection (JSON-RPC 2 client) 
- Automation API (Python API):
`stl-sim` is a program to automate testing. You can use it with the same profile file. Its usecases include: testing traffic profiles before running them, generating output PCAP file, converting stream profile to another type, convert any profile to json. The following command runs traffic profile through simulator, also limiting the number of packets to 10, and storing the output in a PCAP file.
```bash
./stl-sim -f stl/udp_1pkt_simple.py -o b.pcap -l 10
```
other profile output options: `--yaml`, `--json`, `--pkt`
```bash
./stl-sim -f stl/udp_1pkt_simple.py --json
```
for converting to native (python code) profile
```bash
./stl-sim -f my_yaml.yaml --native
```
In trex stateless, we can config operating ports in either l2 or l3 mode.
- in l2 mode, you need to specify only destination MAC address
- in l3 mode, you need to define both src and dest IPs.

https://trex-tgn.cisco.com/trex/doc/trex_console.html

#### Sending large pcap files via stateless-TRex
You should use **TRex 2.79**.

Run TRex server : `./t-rex-64 -i`

Now run `pyton3 final_remote.py`

Another way to send pcap file through a single port:
```bash
python3 /opt/v2.79/automation/trex_control_plane/interactive/trex/examples/stl/pcap_remote.py -f /home/trex/Desktop/SAT-03-11-2018_0137.pcap -p 0
```
### TRex interactive :

```
          | - - - - traffic_profile_1  - - - stream_1
          |                             ...
interface -                            - - - stream_3 : packet_template or Field_Engine
          |
          | - - - - traffic_profile_2
```          
- Each stream can be configure by mode (burst,...), rate, ...
- `Service` mode is for abling to receive RX
- modes: on, off, packet. In packets mode, we can define costumised packets
- We even are able to capture packets by ports, using BPF syntax
- `vm` option is for Field Engine.
