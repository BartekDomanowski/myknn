#include "kdtree.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

static const double* node_idx(const double *data, size_t n_features, size_t idx) {
    return data + idx * n_features;
}

static void swap_node_idx(node *a, node *b) {
    size_t tmp = a->idx;
    a->idx = b->idx;
    b->idx = tmp;
}

static size_t partition_by_axis(node *nodes, size_t left, size_t right, size_t axis, const double *data, size_t n_features) {
    double pivot = node_idx(data, n_features, nodes[right].idx)[axis];
    size_t i = left;
    for (size_t j = left; j < right; j++) {
        if (node_idx(data, n_features, nodes[j].idx)[axis] <= pivot) {
            swap_node_idx(&nodes[i++], &nodes[j]);
        }
    }
    swap_node_idx(&nodes[i], &nodes[right]);
    return i;
}

static node* quickselect(node *nodes, size_t left, size_t right, size_t k, size_t axis, const double *data, size_t n_features) {
    while (left < right) {
        size_t pivot_idx = partition_by_axis(nodes, left, right, axis, data, n_features);
        if (pivot_idx == k) return &nodes[k];
        if (k < pivot_idx) {
            right = pivot_idx - 1;
        } else { // k > pivot_idx
            left = pivot_idx + 1;
        }
    }
    return &nodes[left];
}

static double euclidean_distance_squared(const double *a, const double *b, size_t n_features) {
    double sum = 0.0;
    for (size_t i = 0; i < n_features; i++) {
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return sum;
}

static node* kdtree_build_recursive(node *nodes, size_t left, size_t right, size_t n_features, size_t axis, const double *data) {
    if (left > right) {
        return NULL;
    }
    size_t n = right - left + 1;
    size_t median_idx = left + (n - 1) / 2;
    size_t curr_axis = axis % n_features;
    node *root = quickselect(nodes, left, right, median_idx, curr_axis, data, n_features);
    root->left = (median_idx > left)
        ? kdtree_build_recursive(nodes, left, median_idx - 1, n_features, axis + 1, data)
        : NULL;
    root->right = (median_idx < right)
        ? kdtree_build_recursive(nodes, median_idx + 1, right, n_features, axis + 1, data)
        : NULL;
    return root;
}

static node* kdtree_build_nodes(size_t n_samples) {
    node *nodes = (node*)malloc(n_samples * sizeof(node));
    if (nodes == NULL) return NULL;
    for (size_t i = 0; i < n_samples; i++) {
        nodes[i].idx = i;
        nodes[i].left = NULL;
        nodes[i].right = NULL;
    }
    return nodes;
}

kdtree* kdtree_build(const double *data, size_t n_samples, size_t n_features) {
    if (data == NULL || n_samples == 0 || n_features == 0) return NULL;
    node *nodes = kdtree_build_nodes(n_samples);
    if (nodes == NULL) return NULL;
    kdtree *tree = (kdtree*)malloc(sizeof(kdtree));
    if (tree == NULL) {
        free(nodes);
        return NULL;
    }
    tree->root = kdtree_build_recursive(nodes, 0, n_samples - 1, n_features, 0, data);
    tree->n_samples = n_samples;
    tree->n_features = n_features;
    tree->nodes = nodes;
    tree->data = data;
    return tree;
}

static void kdtree_search(const node *curr, const kdtree *tree, const double *point, size_t depth,
                          size_t *idx_buf, double *d_buf, size_t k, size_t *n) {
    if (curr == NULL) return;

    const double *coord = node_idx(tree->data, tree->n_features, curr->idx);
    double dist = euclidean_distance_squared(point, coord, tree->n_features);
    if (*n < k || dist < d_buf[k - 1]) {
        // insertion sort on k elems assuming that the first k-1 elements are already sorted
        size_t i = (*n < k) ? (*n)++ : k - 1;
        while (i > 0 && d_buf[i - 1] >= dist) {
            idx_buf[i] = idx_buf[i - 1];
            d_buf[i] = d_buf[i - 1];
            i--;
        }
        idx_buf[i] = curr->idx;
        d_buf[i] = dist;
    }

    size_t ax = depth % tree->n_features;
    const node *near = (point[ax] < coord[ax]) ? curr->left : curr->right;
    const node *far = (point[ax] < coord[ax]) ? curr->right : curr->left;
    kdtree_search(near, tree, point, depth + 1, idx_buf, d_buf, k, n);
    double plane = (point[ax] - coord[ax]) * (point[ax] - coord[ax]);
    if (*n < k || plane <= d_buf[k - 1]) { //safe couse of || 
        kdtree_search(far, tree, point, depth + 1, idx_buf, d_buf, k, n);
    }
}

int kdtree_query(const kdtree *tree, const double *point, size_t k, size_t *out_indices, double *out_distances) {
    if (tree == NULL || point == NULL || out_indices == NULL || out_distances == NULL
            || k == 0 || k > tree->n_samples || tree->root == NULL) {
        return -1;
    }
    size_t n = 0;
    kdtree_search(tree->root, tree, point, 0, out_indices, out_distances, k, &n);
    for (size_t i = 0; i < n; i++) {
        out_distances[i] = sqrt(out_distances[i]);
    }
    return 0;
}

void kdtree_free(kdtree *tree) {
    if (tree == NULL) return;
    if (tree->nodes != NULL) free(tree->nodes);
    free(tree);
}