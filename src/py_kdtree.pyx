# distutils: language = c

cimport numpy as np
import numpy as np
from libc.stdlib cimport free, malloc, realloc
from libc.string cimport memcpy
from libc.stddef cimport size_t

np.import_array()

cdef extern from "kdtree.h":
    ctypedef struct kdtree_layout:
        const double *data
        size_t n_samples
        size_t n_features
        size_t sample_stride
        size_t feature_stride

    ctypedef struct kdtree:
        pass

    kdtree_layout kdtree_layout_rowmajor(
        const double *data, size_t n_samples, size_t n_features,
    ) nogil
    kdtree *c_kdtree_build "kdtree_build" (const kdtree_layout *layout) nogil
    ctypedef struct kdtree_radius_result:
        size_t *indices
        double *distances
        size_t count

    int c_kdtree_query "kdtree_query" (
        const kdtree *tree,
        const double *point,
        size_t k,
        size_t *out_indices,
        double *out_distances,
    ) nogil
    int c_kdtree_query_radius "kdtree_query_radius" (
        const kdtree *tree,
        const double *point,
        double r,
        int return_distance,
        kdtree_radius_result *out,
    ) nogil
    void c_kdtree_radius_result_clear "kdtree_radius_result_clear" (
        kdtree_radius_result *out,
    ) nogil
    void c_kdtree_free "kdtree_free" (kdtree *tree) nogil


cdef int _query_c(
    kdtree *tree,
    const double *point,
    size_t k,
    size_t *out_indices,
    double *out_distances,
) nogil:
    return c_kdtree_query(tree, point, k, out_indices, out_distances)


cdef class _KDTreeHandle:
    cdef kdtree *tree
    cdef object data
    cdef size_t n_samples
    cdef size_t n_features
    cdef size_t *idx_buf
    cdef double *dist_buf
    cdef size_t buf_capacity

    def __dealloc__(self):
        if self.idx_buf != NULL:
            free(self.idx_buf)
        if self.dist_buf != NULL:
            free(self.dist_buf)
        if self.tree != NULL:
            c_kdtree_free(self.tree)

    cdef int _ensure_query_bufs(self, size_t k) except -1:
        if k <= self.buf_capacity:
            return 0
        cdef size_t *new_idx = <size_t *>realloc(
            self.idx_buf, k * sizeof(size_t),
        )
        cdef double *new_dist = <double *>realloc(
            self.dist_buf, k * sizeof(double),
        )
        if new_idx == NULL or new_dist == NULL:
            raise MemoryError("failed to allocate query buffers")
        self.idx_buf = new_idx
        self.dist_buf = new_dist
        self.buf_capacity = k
        return 0


def kdtree_build(np.ndarray data not None):
    """
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
    """
    if data.ndim != 2:
        raise ValueError("'data' must be a 2D array")
    cdef np.ndarray[double, ndim=2, mode="c"] cdata = np.ascontiguousarray(
        data, dtype=np.float64,
    )
    if cdata.shape[0] == 0 or cdata.shape[1] == 0:
        raise ValueError("'data' must be non-empty")

    cdef kdtree_layout layout
    cdef _KDTreeHandle handle = _KDTreeHandle()
    handle.data = cdata
    handle.n_samples = <size_t>cdata.shape[0]
    handle.n_features = <size_t>cdata.shape[1]

    layout = kdtree_layout_rowmajor(
        <const double *>np.PyArray_DATA(cdata),
        handle.n_samples,
        handle.n_features,
    )
    handle.tree = c_kdtree_build(&layout)
    if handle.tree == NULL:
        raise MemoryError("failed to build k-d tree")
    return handle


def kdtree_query(
    _KDTreeHandle tree not None,
    np.ndarray point not None,
    int k=1,
    bint return_distance=True,
):
    """
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

    """
    if tree.tree == NULL:
        raise ValueError("invalid k-d tree")

    cdef np.ndarray[double, ndim=1, mode="c"] cpoint
    if (
        np.PyArray_NDIM(point) == 1
        and np.PyArray_TYPE(point) == np.NPY_FLOAT64
        and np.PyArray_IS_C_CONTIGUOUS(point)
    ):
        cpoint = point
    else:
        cpoint = np.ascontiguousarray(point, dtype=np.float64)
    if <size_t>cpoint.shape[0] != tree.n_features:
        raise ValueError(
            f"'point' must have length {tree.n_features}",
        )
    if k < 1:
        raise ValueError("'k' must be a positive integer")
    if <size_t>k > tree.n_samples:
        raise ValueError("'k' cannot exceed the number of training points")

    cdef size_t ck = <size_t>k
    tree._ensure_query_bufs(ck)

    cdef int rc = _query_c(
        tree.tree, &cpoint[0], ck, tree.idx_buf, tree.dist_buf,
    )
    if rc != 0:
        raise RuntimeError("k-d tree query failed")

    cdef np.ndarray idx_arr = np.empty(k, dtype=np.intp)
    cdef np.ndarray dist_arr
    memcpy(
        np.PyArray_DATA(idx_arr),
        tree.idx_buf,
        ck * sizeof(size_t),
    )

    if return_distance:
        dist_arr = np.empty(k, dtype=np.float64)
        memcpy(
            np.PyArray_DATA(dist_arr),
            tree.dist_buf,
            ck * sizeof(double),
        )
        return dist_arr, idx_arr
    return idx_arr


def kdtree_query_radius(
    _KDTreeHandle tree not None,
    np.ndarray point not None,
    double r,
    bint return_distance=False,
):
    """
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
    """
    if tree.tree == NULL:
        raise ValueError("invalid k-d tree")
    if r < 0.0:
        raise ValueError("'r' must be non-negative")

    cdef np.ndarray[double, ndim=1, mode="c"] cpoint
    if (
        np.PyArray_NDIM(point) == 1
        and np.PyArray_TYPE(point) == np.NPY_FLOAT64
        and np.PyArray_IS_C_CONTIGUOUS(point)
    ):
        cpoint = point
    else:
        cpoint = np.ascontiguousarray(point, dtype=np.float64)
    if <size_t>cpoint.shape[0] != tree.n_features:
        raise ValueError(
            f"'point' must have length {tree.n_features}",
        )

    cdef kdtree_radius_result res
    res.indices = NULL
    res.distances = NULL
    res.count = 0

    cdef int rc
    with nogil:
        rc = c_kdtree_query_radius(
            tree.tree, &cpoint[0], r, <int>return_distance, &res,
        )
    if rc != 0:
        raise RuntimeError("k-d tree radius query failed")

    cdef size_t n = res.count
    cdef np.ndarray idx_arr
    cdef np.ndarray dist_arr

    if n == 0:
        idx_arr = np.empty(0, dtype=np.intp)
        c_kdtree_radius_result_clear(&res)
        if return_distance:
            dist_arr = np.empty(0, dtype=np.float64)
            return idx_arr, dist_arr
        return idx_arr

    idx_arr = np.empty(n, dtype=np.intp)
    memcpy(np.PyArray_DATA(idx_arr), res.indices, n * sizeof(size_t))

    if return_distance:
        dist_arr = np.empty(n, dtype=np.float64)
        memcpy(np.PyArray_DATA(dist_arr), res.distances, n * sizeof(double))
        c_kdtree_radius_result_clear(&res)
        return idx_arr, dist_arr

    c_kdtree_radius_result_clear(&res)
    return idx_arr
