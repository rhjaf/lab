
sudo ./dpdk_setup_ports.py -c 02:01.0 02:06.0
we can use interface's name instead of MAC.
low end laptio. we can use threads assigned to specific cores.
Your system should have at least 3 cores for 2 interfaces, and it has: 2
After doing all configs, you can run basic TRex for 10 seconds, using the following command and it will use the default config file in /etc/trex_cfg.yaml:
 sudo ./t-rex-64 -f cap2/dns.yaml -c 4 -m 1 -d 10
