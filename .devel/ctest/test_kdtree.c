#include "../../src/kdtree.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static void expect_knn(kdtree *tree, const double *point, size_t k,
                       const size_t *want_idx, const double *want_dist) {
    size_t got_idx[8];
    double got_dist[8];
    assert(k <= 8);
    assert(kdtree_query(tree, point, k, got_idx, got_dist) == 0);
    for (size_t i = 0; i < k; i++) {
        assert(got_idx[i] == want_idx[i]);
        assert(fabs(got_dist[i] - want_dist[i]) < 1e-9);
    }
}

static void test_1d_nearest_easy(void) {
    const double data[] = {0.0, 10.0, 20.0};
    kdtree_layout layout = kdtree_layout_rowmajor(data, 3, 1);
    kdtree *tree = kdtree_build(&layout);
    assert(tree != NULL);
    const double query[] = {1.0};
    const size_t want_idx[] = {0};
    const double want_dist[] = {1.0};
    expect_knn(tree, query, 1, want_idx, want_dist);
    kdtree_free(tree);
}

static void test_query_on_training_point(void) {
    const double data[] = {
        0.0, 0.0,
        3.0, 4.0,
        100.0, 0.0,
    };
    kdtree_layout layout = kdtree_layout_rowmajor(data, 3, 2);
    kdtree *tree = kdtree_build(&layout);
    assert(tree != NULL);
    const double query[] = {0.0, 0.0};
    const size_t want_idx[] = {0};
    const double want_dist[] = {0.0};
    expect_knn(tree, query, 1, want_idx, want_dist);
    kdtree_free(tree);
}

static void test_two_closest(void) {
    const double data[] = {
        0.0, 0.0,
        1.0, 0.0,
        100.0, 0.0,
    };
    kdtree_layout layout = kdtree_layout_rowmajor(data, 3, 2);
    kdtree *tree = kdtree_build(&layout);
    assert(tree != NULL);
    const double query[] = {0.0, 0.0};
    const size_t want_idx[] = {0, 1};
    const double want_dist[] = {0.0, 1.0};
    expect_knn(tree, query, 2, want_idx, want_dist);
    kdtree_free(tree);
}

static void test_colmajor_layout(void) {
    const double data[] = {
        0.0, 1.0, 100.0,
        0.0, 0.0, 0.0,
    };
    kdtree_layout layout = kdtree_layout_colmajor(data, 3, 2);
    kdtree *tree = kdtree_build(&layout);
    assert(tree != NULL);
    const double query[] = {0.0, 0.0};
    const size_t want_idx[] = {0, 1};
    const double want_dist[] = {0.0, 1.0};
    expect_knn(tree, query, 2, want_idx, want_dist);
    kdtree_free(tree);
}

static void test_radius_subset(void) {
    const double data[] = {0.0, 0.0, 1.0, 0.0, 100.0, 0.0};
    kdtree_layout layout = kdtree_layout_rowmajor(data, 3, 2);
    kdtree *tree = kdtree_build(&layout);
    assert(tree != NULL);
    const double query[] = {0.0, 0.0};
    kdtree_radius_result res = {0};
    assert(kdtree_query_radius(tree, query, 1.5, 0, &res) == 0);
    assert(res.count == 2);
    assert(res.indices[0] == 1);
    assert(res.indices[1] == 0);
    kdtree_radius_result_clear(&res);
    kdtree_free(tree);
}

static void test_radius_zero_at_point(void) {
    const double data[] = {0.0, 0.0, 1.0, 0.0, 100.0, 0.0};
    kdtree_layout layout = kdtree_layout_rowmajor(data, 3, 2);
    kdtree *tree = kdtree_build(&layout);
    assert(tree != NULL);
    const double query[] = {0.0, 0.0};
    kdtree_radius_result res = {0};
    assert(kdtree_query_radius(tree, query, 0.0, 1, &res) == 0);
    assert(res.count == 1);
    assert(res.indices[0] == 0);
    assert(res.distances[0] == 0.0);
    kdtree_radius_result_clear(&res);
    kdtree_free(tree);
}


int main(void) {
    test_1d_nearest_easy();
    test_query_on_training_point();
    test_two_closest();
    test_radius_subset();
    test_radius_zero_at_point();
    test_colmajor_layout();
    return 0;
}
