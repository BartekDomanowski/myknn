#include "kdtree.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

static void swap_node_idx(node *a, node *b) {
    size_t tmp = a->idx;
    a->idx = b->idx;
    b->idx = tmp;
}

static double layout_axis_value(const kdtree_layout *layout, size_t sample, size_t axis) {
    return kdtree_get(layout, sample, axis);
}

static size_t partition_by_axis(node *nodes, size_t left, size_t right, size_t axis, const kdtree_layout *layout) {
    double pivot = layout_axis_value(layout, nodes[right].idx, axis);
    size_t i = left;
    for (size_t j = left; j < right; j++) {
        if (layout_axis_value(layout, nodes[j].idx, axis) <= pivot) {
            swap_node_idx(&nodes[i++], &nodes[j]);
        }
    }
    swap_node_idx(&nodes[i], &nodes[right]);
    return i;
}

static node *quickselect(node *nodes, size_t left, size_t right, size_t k, size_t axis, const kdtree_layout *layout) {
    while (left < right) {
        size_t pivot_idx = partition_by_axis(nodes, left, right, axis, layout);
        if (pivot_idx == k) return &nodes[k];
        if (k < pivot_idx) {
            right = pivot_idx - 1;
        } else {
            left = pivot_idx + 1;
        }
    }
    return &nodes[left];
}

static double euclidean_distance_squared(const kdtree_layout *layout, size_t sample, const double *point) {
    double sum = 0.0;
    for (size_t j = 0; j < layout->n_features; j++) {
        double diff = point[j] - kdtree_get(layout, sample, j);
        sum += diff * diff;
    }
    return sum;
}

static node *kdtree_build_recursive(node *nodes, size_t left, size_t right, size_t n_features, size_t axis, const kdtree_layout *layout) {
    if (left > right) {
        return NULL;
    }
    size_t n = right - left + 1;
    size_t median_idx = left + (n - 1) / 2;
    size_t curr_axis = axis % n_features;
    node *root = quickselect(nodes, left, right, median_idx, curr_axis, layout);
    root->left = (median_idx > left)
        ? kdtree_build_recursive(nodes, left, median_idx - 1, n_features, axis + 1, layout)
        : NULL;
    root->right = (median_idx < right)
        ? kdtree_build_recursive(nodes, median_idx + 1, right, n_features, axis + 1, layout)
        : NULL;
    return root;
}

static node *kdtree_build_nodes(size_t n_samples) {
    node *nodes = (node *)malloc(n_samples * sizeof(node));
    if (nodes == NULL) return NULL;
    for (size_t i = 0; i < n_samples; i++) {
        nodes[i].idx = i;
        nodes[i].left = NULL;
        nodes[i].right = NULL;
    }
    return nodes;
}

kdtree *kdtree_build(const kdtree_layout *layout) {
    if (layout == NULL || layout->data == NULL || layout->n_samples == 0 || layout->n_features == 0) {
        return NULL;
    }
    node *nodes = kdtree_build_nodes(layout->n_samples);
    if (nodes == NULL) return NULL;
    kdtree *tree = (kdtree *)malloc(sizeof(kdtree));
    if (tree == NULL) {
        free(nodes);
        return NULL;
    }
    tree->layout = *layout;
    tree->root = kdtree_build_recursive(nodes, 0, layout->n_samples - 1, layout->n_features, 0, layout);
    tree->nodes = nodes;
    return tree;
}

static void kdtree_search(const node *curr, const kdtree *tree, const double *point, size_t depth,
                          size_t *idx_buf, double *d_buf, size_t k, size_t *n) {
    if (curr == NULL) return;

    double dist = euclidean_distance_squared(&tree->layout, curr->idx, point);
    if (*n < k || dist < d_buf[k - 1]) {
        size_t i = (*n < k) ? (*n)++ : k - 1;
        while (i > 0 && d_buf[i - 1] >= dist) {
            idx_buf[i] = idx_buf[i - 1];
            d_buf[i] = d_buf[i - 1];
            i--;
        }
        idx_buf[i] = curr->idx;
        d_buf[i] = dist;
    }

    size_t ax = depth % tree->layout.n_features;
    double coord_ax = kdtree_get(&tree->layout, curr->idx, ax);
    const node *near = (point[ax] < coord_ax) ? curr->left : curr->right;
    const node *far = (point[ax] < coord_ax) ? curr->right : curr->left;
    kdtree_search(near, tree, point, depth + 1, idx_buf, d_buf, k, n);
    double plane = (point[ax] - coord_ax) * (point[ax] - coord_ax);
    if (*n < k || plane <= d_buf[k - 1]) {
        kdtree_search(far, tree, point, depth + 1, idx_buf, d_buf, k, n);
    }
}

int kdtree_query(const kdtree *tree, const double *point, size_t k, size_t *out_indices, double *out_distances) {
    if (tree == NULL || point == NULL || out_indices == NULL || out_distances == NULL
            || k == 0 || k > tree->layout.n_samples || tree->root == NULL) {
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