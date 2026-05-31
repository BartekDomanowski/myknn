# distutils: language = c

cimport numpy as np
import numpy as np
from libc.stdlib cimport free, malloc
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
    int c_kdtree_query "kdtree_query" (
        const kdtree *tree,
        const double *point,
        size_t k,
        size_t *out_indices,
        double *out_distances,
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

    def __dealloc__(self):
        if self.tree != NULL:
            c_kdtree_free(self.tree)


def kdtree_build(np.ndarray data not None):
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
    if tree.tree == NULL:
        raise ValueError("invalid k-d tree")

    cdef np.ndarray[double, ndim=1, mode="c"] cpoint = np.ascontiguousarray(
        point, dtype=np.float64,
    )
    if <size_t>cpoint.shape[0] != tree.n_features:
        raise ValueError(
            f"'point' must have length {tree.n_features}",
        )
    if k < 1:
        raise ValueError("'k' must be a positive integer")
    if <size_t>k > tree.n_samples:
        raise ValueError("'k' cannot exceed the number of training points")

    cdef size_t *idx_buf = <size_t *>malloc(<size_t>k * sizeof(size_t))
    cdef double *dist_buf = <double *>malloc(<size_t>k * sizeof(double))
    if idx_buf == NULL or dist_buf == NULL:
        free(idx_buf)
        free(dist_buf)
        raise MemoryError("failed to allocate query buffers")

    cdef int rc
    cdef size_t i
    cdef np.ndarray idx_arr
    cdef np.ndarray dist_arr
    try:
        rc = _query_c(tree.tree, &cpoint[0], <size_t>k, idx_buf, dist_buf)
        if rc != 0:
            raise RuntimeError("k-d tree query failed")

        idx_arr = np.empty(k, dtype=np.intp)
        for i in range(<size_t>k):
            idx_arr[i] = idx_buf[i]

        if return_distance:
            dist_arr = np.empty(k, dtype=np.float64)
            for i in range(<size_t>k):
                dist_arr[i] = dist_buf[i]
            return dist_arr, idx_arr
        return idx_arr
    finally:
        free(idx_buf)
        free(dist_buf)
