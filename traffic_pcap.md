The following repo, generates flow from packets
https://github.com/caesar0301/pkt2flow

To replay pcap trrafic as many times you want:
```bash
tcpreplay --multiplier
Push -f pcap (from disk) -I (port) --times [how many time to repeat] --memory (load to memory) --min-ipg (usec )
```

```bash
tcpreplay -i ens160 -t -K --loop 1 my_pcap.pcap
```
