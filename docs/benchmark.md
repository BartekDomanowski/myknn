---
title: Benchmarks
---

# Benchmarks

**sklearn** `neighbors.KDTree` vs **airroute** (`kdtree_build`, `kdtree_query`, `kdtree_query_radius`).

- `n_samples` ∈ {25 000, 500 000, 1 000 000}
- `n_features` ∈ {2, 5}

`X`: `n_samples` × `n_features`, i.i.d. standard normal, `float64`.  
Mean time over **100** runs (seconds). `k = 5`, **128** queries per loop (one point per call). For **query radius**, fixed Euclidean **`r = 0.15`** when `n_features = 2` and **`r = 0.5`** when `n_features = 5` (same `r` for all `n_samples` in that column).

**Time is in seconds.** **Less is better.**

### Build

| n_samples | n_features | sklearn | airroute |
| ---: | ---: | ---: | ---: |
| 25000 | 2 | 0.0024 | 0.0026 |
| 25000 | 5 | 0.0035 | 0.0030 |
| 500000 | 2 | 0.0820 | 0.0754 |
| 500000 | 5 | 0.1194 | 0.0864 |
| 1000000 | 2 | 0.1856 | 0.1654 |
| 1000000 | 5 | 0.3226 | 0.1996 |

### Query

| n_samples | n_features | sklearn | airroute |
| ---: | ---: | ---: | ---: |
| 25000 | 2 | 0.0015 | 0.0001 |
| 25000 | 5 | 0.0022 | 0.0007 |
| 500000 | 2 | 0.0015 | 0.0001 |
| 500000 | 5 | 0.0024 | 0.0011 |
| 1000000 | 2 | 0.0016 | 0.0001 |
| 1000000 | 5 | 0.0031 | 0.0014 |

### Query radius, r = 0.15 (2D) / r = 0.5 (5D)

| n_samples | n_features | sklearn | airroute |
| ---: | ---: | ---: | ---: |
| 25000 | 2 | 0.0025 | 0.0004 |
| 25000 | 5 | 0.0034 | 0.0009 |
| 500000 | 2 | 0.0198 | 0.0068 |
| 500000 | 5 | 0.0193 | 0.0158 |
| 1000000 | 2 | 0.0275 | 0.0281 |
| 1000000 | 5 | 0.0468 | 0.0350 |