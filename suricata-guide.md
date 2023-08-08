Simple suricata rule
```yaml
alert http any any -> any any (msg:"winhttp";http.user_agent;content:"winhttp";sid:1;rev:1)
```
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
You can use `jq` to parse JSON files (like eve.json log file) with specific queries
```bash
jq 'select(.event_type=="http").http.http_user_agent'
```

