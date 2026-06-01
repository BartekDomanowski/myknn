import numpy as np
import pytest

from airroute import kdtree_build, kdtree_query


def expect_knn(tree, point, k, want_idx, want_dist, return_distance=True, tol=1e-9):
    if return_distance:
        dist, idx = kdtree_query(tree, point, k, return_distance=True)
        np.testing.assert_array_equal(idx, want_idx)
        np.testing.assert_allclose(dist, want_dist, rtol=0, atol=tol)
    else:
        idx = kdtree_query(tree, point, k, return_distance=False)
        np.testing.assert_array_equal(idx, want_idx)


def test_1d_nearest():
    data = np.array([[0.0], [10.0], [20.0]])
    tree = kdtree_build(data)
    expect_knn(tree, np.array([1.0]), 1, [0], [1.0])
    expect_knn(tree, np.array([1.0]), 1, [0], [1.0], return_distance=False)


def test_query_on_training_point():
    data = np.array([[0.0, 0.0], [3.0, 4.0], [100.0, 0.0]])
    tree = kdtree_build(data)
    expect_knn(tree, np.array([0.0, 0.0]), 1, [0], [0.0])


def test_two_closest():
    data = np.array([[0.0, 0.0], [1.0, 0.0], [100.0, 0.0]])
    tree = kdtree_build(data)
    expect_knn(tree, np.array([0.0, 0.0]), 2, [0, 1], [0.0, 1.0])
    idx = kdtree_query(tree, np.array([0.0, 0.0]), 2, return_distance=False)
    np.testing.assert_array_equal(idx, [0, 1])


def test_return_distance_false_is_not_tuple():
    data = np.array([[0.0, 0.0], [1.0, 0.0]])
    tree = kdtree_build(data)
    result = kdtree_query(tree, np.array([0.0, 0.0]), 1, return_distance=False)
    assert isinstance(result, np.ndarray)
    assert not isinstance(result, tuple)


def test_k_equals_n():
    data = np.array([[0.0, 0.0], [1.0, 0.0], [100.0, 0.0]])
    tree = kdtree_build(data)
    dist, idx = kdtree_query(tree, np.array([0.0, 0.0]), k=3, return_distance=True)
    assert len(idx) == 3
    assert len(dist) == 3


def test_k_too_large_raises():
    data = np.array([[0.0, 0.0], [1.0, 0.0]])
    tree = kdtree_build(data)
    with pytest.raises(ValueError, match="cannot exceed"):
        kdtree_query(tree, np.array([0.0, 0.0]), k=3)


def test_matches_sklearn_on_small_set():
    pytest.importorskip("sklearn.neighbors")
    from sklearn.neighbors import KDTree as SklearnKDTree

    rng = np.random.RandomState(42)
    X = rng.randn(20, 3).astype(np.float64)
    q = X[5]
    k = 3

    tree = kdtree_build(X)
    dist, ind = kdtree_query(tree, q, k=k, return_distance=True)

    sk_dist, sk_ind = SklearnKDTree(X).query(q.reshape(1, -1), k=k)
    np.testing.assert_array_equal(ind, sk_ind[0])
    np.testing.assert_allclose(dist, sk_dist[0], rtol=0, atol=1e-12)

