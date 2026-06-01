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
| 25000 | 2 | 0.0026 | 0.0029 |
| 25000 | 5 | 0.0043 | 0.0034 |
| 500000 | 2 | 0.0948 | 0.0845 |
| 500000 | 5 | 0.1550 | 0.1019 |
| 1000000 | 2 | 0.2238 | 0.1882 |
| 1000000 | 5 | 0.3939 | 0.2229 |

### Query

| n_samples | n_features | sklearn | airroute |
| ---: | ---: | ---: | ---: |
| 25000 | 2 | 0.0016 | 0.0001 |
| 25000 | 5 | 0.0025 | 0.0008 |
| 500000 | 2 | 0.0026 | 0.0001 |
| 500000 | 5 | 0.0035 | 0.0019 |
| 1000000 | 2 | 0.0017 | 0.0001 |
| 1000000 | 5 | 0.0041 | 0.0020 |

### Query radius, r = 0.15 (2D) / r = 0.5 (5D)

| n_samples | n_features | sklearn | airroute |
| ---: | ---: | ---: | ---: |
| 25000 | 2 | 0.0025 | 0.0007 |
| 25000 | 5 | 0.0033 | 0.0009 |
| 500000 | 2 | 0.0128 | 0.0062 |
| 500000 | 5 | 0.0186 | 0.0168 |
| 1000000 | 2 | 0.0254 | 0.0178 |
| 1000000 | 5 | 0.0445 | 0.0359 |