---
title: Home
---

# AirRoute

k-d tree for k-nearest neighbours (R + Python, C core).

## About

Shared **C** implementation (`src/kdtree.c`), **R** bindings (column-major matrices), **Python** via Cython (row-major `float64`). Benchmarks vs scikit-learn: [Benchmarks](benchmark.html).

## Installation

```bash
git clone https://github.com/BartekDomanowski/AirRoute.git
cd AirRoute
pip install -e ".[dev]"   # Python
R CMD INSTALL .           # R
```
### AirRoute (Python)

```python
import numpy as np
from airroute import kdtree_build, kdtree_query

rng = np.random.RandomState(0)
X = rng.random_sample((10, 3)).astype(np.float64)
tree = kdtree_build(X)
dist, ind = kdtree_query(tree, X[0], k=3, return_distance=True)
print(ind)
print(dist)
```

**Output:**

```
[0 3 1]
[0.         0.19662693 0.29473397]
```

```python
ind = kdtree_query(tree, X[0], k=3, return_distance=False)
print(ind)
```

**Output:**

```
[0 3 1]
```

### AirRoute (R)

```r
library(AirRoute)
X <- matrix(c(
  0.54881350, 0.71518937, 0.60276338,
  0.54488318, 0.42365480, 0.64589411,
  0.43758721, 0.89177300, 0.96366276,
  0.38344152, 0.79172504, 0.52889492,
  0.56804456, 0.92559664, 0.07103606,
  0.08712930, 0.02021840, 0.83261985,
  0.77815675, 0.87001215, 0.97861834,
  0.79915856, 0.46147936, 0.78052918,
  0.11827443, 0.63992102, 0.14335329,
  0.94466892, 0.52184832, 0.41466194
), ncol = 3, byrow = TRUE)
tree <- kdtree_build(X)
kdtree_query(tree, X[1, ], k = 3)
```

**Output:**

```
$indices
[1] 1 4 2

$distances
[1] 0.0000000 0.1966269 0.2947340
```

## Tests

```bash
make test
```

## Repository

[https://github.com/BartekDomanowski/AirRoute](https://github.com/BartekDomanowski/AirRoute)

## License

AGPL-3 — see [LICENSE](https://github.com/BartekDomanowski/AirRoute/blob/main/LICENSE).