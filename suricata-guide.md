Simple suricata rule
```yaml
alert http any any -> any any (msg:"winhttp";http.user_agent;content:"winhttp";sid:1;rev:1)
```
to capture traffic only from a specific NIC, use `-i eth0`

To test your config file, whether it is working or not
```bash
suricata -T -S winhttp.rules -l data/
```
To analyze your signature rules
```bash
suricata --engine-analysis -S winhttp.rules -l data/
```
run in offline-mode. reads from pcap
  - `-l` : log directory
  - `-S` : signature file to load and use it exclusively
```bash
suricata -r tcp.pcap -l data/ -S /dev/null
```
Supported keywords to use in signature files
```bash
suricata --list-keywords
suricata --list-keywords=http_user_agent
```
Suricata generating logs:
- `suricata.log`: Suricata startup messages
- `stats.log`: statistics on your network traffic 
- `fast.log`: suspicious activities detected by Suricata
- `eve.json`: your local network traffic and suspicious activities in JSON format

Default log directory can be in one of the following(you can check it in yaml config file): 
- `/var/log/suricata/`
- `/usr/local/var/log/suricata/`

You can use `jq` to parse JSON files (like eve.json log file) with specific queries
```bash
jq 'select(.event_type=="http").http.http_user_agent'
```
Increasing the `ring_size` in config file may lead to smaller packet drops

