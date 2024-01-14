### PromQL queries:
CPU usage
```sql
100 - (avg by (instance) (irate(node_cpu_seconds_total{job="node",mode="idle"}[5m])) * 100)
```
Push DPDK driver NIC metrics to exporter : `tele.py`
