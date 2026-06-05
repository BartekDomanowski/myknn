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
    Tree returned by :func:`kdtree_build`.
point : ndarray, shape (n_features,)
    Query point. Must be ``float64``.
k : int, default 1
    Number of neighbours (positive, at most ``n_samples`` at build time).
return_distance : bool, default True
    If ``True``, return ``(distances, indices)`` like ``sklearn.neighbors.KDTree.query``. 
    If ``False``, return only ``indices``.

Returns
-------
    
distances, indices : ndarray
    When ``return_distance=True``: Euclidean distances and row
    indices into the training data.
indices : ndarray
    When ``return_distance=False``: indices only.
```

### `kdtree_query_radius`

```
tree = kdtree_build(X)
kdtree_query_radius(tree, point, r, return_distance=False)
Query neighbours within Euclidean distance ``r`` of a point.

Parameters
----------

tree
    Tree returned by :func:`kdtree_build`.

point : ndarray, shape (n_features,)
    Query point. Must be ``float64``.

r : float
    Radius; neighbours with distance ``<= r`` are returned.

return_distance : bool, default False
    If ``True``, return ``(indices, distances)``
    If ``False``, return only ``indices``.

Returns
-------

indices : ndarray
    When ``return_distance=False``: row indices into training data.
indices, distances : ndarray
    When ``return_distance=True``: indices and Euclidean distances unsorted
```

## R API

### `kdtree_build`

```
Build a k-d tree from a numeric matrix

Builds a k-d tree over \code{n} points in \code{d} dimensions. Rows of
\code{data} are samples; columns are features. Distances are Euclidean
in that feature space.

@param data numeric matrix (\code{n} rows, \code{d} columns).
@return an external pointer of class \code{myknn_kdtree}; freed
automatically when garbage-collected.
```

### `kdtree_query`

```
Query k nearest neighbours

@param tree object returned by [kdtree_build].
@param point numeric vector of length \code{ncol(data)} used at build time.
@param k number of neighbours (positive integer, at most \code{nrow(data)}).
@return a list with \code{indices} (1-based row indices into \code{data})
  and \code{distances} (Euclidean, same units as \code{data}).
```

### `kdtree_query_radius`

```
Query neighbours within a radius

@param tree object returned by [kdtree_build].
@param point numeric vector of length \code{ncol(data)} used at build time.
@param r non-negative radius (Euclidean distance).
@param return_distance if \code{TRUE}, return a list with
  \code{indices} (1-based) and \code{distances}; if \code{FALSE}, return
  only the index vector.
@return integer vector of 1-based row indices, or a named list when
  \code{return_distance = TRUE}.
```
