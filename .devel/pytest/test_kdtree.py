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

