# AirRoute

About
------------
R and Python package to speedup Scikit Learn knn algos in certain situations
The core is implemented in C with R and Python bindings. The Python
``kdtree_query`` API follows ``sklearn.neighbors.KDTree.query`` conventions
(e.g. ``return_distance``); 
this is not a full scikit-learn replacement.

Installation
------------
```bash
# Python
pip install -e ".[dev]"
# R
R CMD INSTALL .
```

Quick example
------------
**Python:**
```python
import numpy as np
from airroute import kdtree_build, kdtree_query

data = np.array([[0, 0], [1, 0], [100, 0]], dtype=float)
tree = kdtree_build(data)
dist, idx = kdtree_query(tree, np.array([0.0, 0.0]), k=2, return_distance=False)
# [0, 1]
```

**R:**
```r
library(AirRoute)
data <- rbind(c(0,0), c(1,0), c(100,0))
tree <- kdtree_build(data)
kdtree_query(tree, c(0, 0), k = 2)
# [1, 2]
```

Tests
------------
```bash
make test          
```

Repository stucture
------------
Repository: https://github.com/BartekDomanowski/AirRoute

License
------------
AGPL-3 — see [LICENSE](LICENSE).
