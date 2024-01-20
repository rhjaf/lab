## Promethus
Stores metrics as `timestamps` which are defined by specific name and values: `metric_name{key=value,k2=v2}` 
Data types:
  - scalar
      - float
      - string
  - instance vector
      - binary opertors are only appliable on them
  - range vector:
      - over a time
      - use operator_name + `_over`, e.g: `sum_over`
### promql
- Search by metric's name and also labels.
- `cpu {....} offset 10ms` will return instance vector of 10ms ago
