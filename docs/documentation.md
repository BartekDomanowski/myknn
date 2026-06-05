---
title: Documentation
---

# Documentation

## Python API

### `kdtree_build`

```
kdtree_build(X)
Build a k-d tree over training points X matrix n_samples * n_features.

Parameters
----------

data : ndarray, shape (n_samples, n_features)
    Training data. Must be float64 and C-contiguous (row-major).

Returns
-------

tree
    KDTree structure to be used in queries.
```

### `kdtree_query`

```
tree = kdtree_build(X)
kdtree_query(tree, point, k, return_distance=True)
Query k nearest neighbours of a given point.

Parameters
----------

tree
    Tree returned by kdtree_build.
point : ndarray, shape (n_features,)
    Query point. Must be float64.
k : int, default 1
    Number of neighbours (positive, at most n_samples at build time).
return_distance : bool, default True
    If True, return (distances, indices) like sklearn.neighbors.KDTree.query.
    If False, return only indices.

Returns
-------
    
distances, indices : ndarray
    When return_distance=True: Euclidean distances and row
    indices into the training data.
indices : ndarray
    When return_distance=False: indices only.
```

### `kdtree_query_radius`

```
tree = kdtree_build(X)
kdtree_query_radius(tree, point, r, return_distance=False)
Query neighbours within Euclidean distance r of a point.

Parameters
----------

tree
    Tree returned by kdtree_build.

point : ndarray, shape (n_features,)
    Query point. Must be float64.

r : float
    Radius; neighbours with distance <= r are returned.

return_distance : bool, default False
    If True, return (indices, distances)
    If False, return only indices.

Returns
-------

indices : ndarray
    When return_distance=False: row indices into training data.
indices, distances : ndarray
    When return_distance=True: indices and Euclidean distances unsorted
```

## R API

### `kdtree_build`

```
kdtree_build(data)
Build a k-d tree from a numeric matrix.

Builds a k-d tree over n points in d dimensions. Rows of data are samples;
columns are features. Distances are Euclidean in that feature space.

Parameters
----------

data : numeric matrix (n rows, d columns)
    Training data.

Returns
-------

tree
    External pointer of class myknn_kdtree; freed automatically when
    garbage-collected.
```

### `kdtree_query`

```
kdtree_query(tree, point, k)
Query k nearest neighbours.

Parameters
----------

tree
    Object returned by kdtree_build.
point : numeric vector
    Length ncol(data) used at build time.
k : int
    Number of neighbours (positive integer, at most nrow(data)).

Returns
-------

list
    indices — 1-based row indices into data
    distances — Euclidean, same units as data
```

### `kdtree_query_radius`

```
kdtree_query_radius(tree, point, r, return_distance = FALSE)
Query neighbours within a radius.

Parameters
----------

tree
    Object returned by kdtree_build.
point : numeric vector
    Length ncol(data) used at build time.
r : numeric
    Non-negative radius (Euclidean distance).
return_distance : logical, default FALSE
    If TRUE, return a list with indices (1-based) and distances.
    If FALSE, return only the index vector.

Returns
-------

indices : integer vector
    When return_distance = FALSE: 1-based row indices into training data.
indices, distances : list
    When return_distance = TRUE: named list with indices and distances.
```
