#include "../../src/kdtree.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

//helper to check if the nearest k points are correct
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

// punkty x = 0, 10, 20; zapytanie x = 1 → (0,1)
static void test_1d_nearest_easy(void) {
    const double data[] = {0.0, 10.0, 20.0};
    kdtree *tree = kdtree_build(data, 3, 1);
    assert(tree != NULL);
    const double query[] = {1.0};
    const size_t want_idx[] = {0};
    const double want_dist[] = {1.0};
    expect_knn(tree, query, 1, want_idx, want_dist);
    kdtree_free(tree);
}

// zapytanie = punkt treningowy (0,0) → (0,0)
static void test_query_on_training_point(void) {
    const double data[] = {
        0.0, 0.0,
        3.0, 4.0,
        100.0, 0.0,
    };
    kdtree *tree = kdtree_build(data, 3, 2);
    assert(tree != NULL);
    const double query[] = {0.0, 0.0};
    const size_t want_idx[] = {0};
    const double want_dist[] = {0.0};
    expect_knn(tree, query, 1, want_idx, want_dist);
    kdtree_free(tree);
}

// (0,0), (1,0), (100,0); zapytanie (0,0), k=2 → (0,0), (1,0)
static void test_two_closest(void) {
    const double data[] = {
        0.0, 0.0,
        1.0, 0.0,
        100.0, 0.0,
    };
    kdtree *tree = kdtree_build(data, 3, 2);
    assert(tree != NULL);
    const double query[] = {0.0, 0.0};
    const size_t want_idx[] = {0, 1};
    const double want_dist[] = {0.0, 1.0};
    expect_knn(tree, query, 2, want_idx, want_dist);
    kdtree_free(tree);
}

int main(void) {
    test_1d_nearest_easy();
    test_query_on_training_point();
    test_two_closest();
    return 0;
}
