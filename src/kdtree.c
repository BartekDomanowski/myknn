#include "kdtree.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

static double kdtree_get(const kdtree_layout *layout, size_t sample, size_t feature) {
    return layout->data[sample * layout->sample_stride + feature * layout->feature_stride];
}

kdtree_layout kdtree_layout_rowmajor(const double *data, size_t n_samples, size_t n_features) {
    return (kdtree_layout){data, n_samples, n_features, n_features, 1};
}

kdtree_layout kdtree_layout_colmajor(const double *data, size_t n_samples, size_t n_features) {
    return (kdtree_layout){data, n_samples, n_features, 1, n_samples};
}

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

typedef struct _radius_buf {
    size_t *indices;
    double *distances;
    size_t count;
    size_t capacity;
    int store_distance;
} radius_buf;
static int radius_buf_push(radius_buf *buf, size_t idx, double dist_sq) {
    if (buf->count >= buf->capacity) return -1;
    buf->indices[buf->count] = idx;
    if (buf->store_distance) buf->distances[buf->count] = sqrt(dist_sq);
    buf->count++;
    return 0;
}

static int kdtree_radius_search(const node *curr, const kdtree *tree, const double *point, size_t depth, double r_sq, radius_buf *buf) {
    if (curr == NULL) return 0;
    double dist_sq = euclidean_distance_squared(&tree->layout, curr->idx, point);
    if (dist_sq <= r_sq) {
        if (radius_buf_push(buf, curr->idx, dist_sq) != 0) return -1;
    }
    size_t ax = depth % tree->layout.n_features;
    double coord_ax = kdtree_get(&tree->layout, curr->idx, ax);
    const node *near = (point[ax] < coord_ax) ? curr->left : curr->right;
    const node *far = (point[ax] < coord_ax) ? curr->right : curr->left;
    if (kdtree_radius_search(near, tree, point, depth + 1, r_sq, buf) != 0) return -1;
    double plane = (point[ax] - coord_ax) * (point[ax] - coord_ax);
    if (plane <= r_sq) {
        if (kdtree_radius_search(far, tree, point, depth + 1, r_sq, buf) != 0) return -1;
    }
    return 0;
}

void kdtree_radius_result_clear(kdtree_radius_result *out) {
    if (out == NULL) return;
    if (out->indices != NULL) {
        free(out->indices);
        out->indices = NULL;
    }
    if (out->distances != NULL) {
        free(out->distances);
        out->distances = NULL;
    }
    out->count = 0;
}

int kdtree_query_radius_buf(const kdtree *tree, const double *point, double r, int return_distance, size_t *indices, size_t capacity, size_t *out_count, double *distances) {
    if (tree == NULL || point == NULL || indices == NULL || out_count == NULL|| tree->root == NULL || r < 0.0 || capacity < tree->layout.n_samples) return -1;
    if (return_distance && distances == NULL) return -1;
    radius_buf buf = {indices, return_distance ? distances : NULL, 0, capacity, return_distance != 0};
    double r_sq = r * r;
    if (kdtree_radius_search(tree->root, tree, point, 0, r_sq, &buf) != 0) return -1;
    *out_count = buf.count;
    return 0;
}

int kdtree_query_radius(const kdtree *tree, const double *point, double r, int return_distance, kdtree_radius_result *out) {
    if (tree == NULL || point == NULL || out == NULL || tree->root == NULL || r < 0.0) return -1;
    size_t n = tree->layout.n_samples;
    size_t *indices = (size_t *)malloc(n * sizeof(size_t));
    if (indices == NULL) return -1;
    double *distances = NULL;
    if (return_distance) {
        distances = (double *)malloc(n * sizeof(double));
        if (distances == NULL) {
            free(indices);
            return -1;
        }
    }
    size_t count = 0;
    if (kdtree_query_radius_buf(tree, point, r, return_distance, indices, n, &count, distances) != 0) {
        free(indices);
        free(distances);
        return -1;
    }
    size_t *out_idx = (size_t *)realloc(indices, count * sizeof(size_t));
    if (out_idx == NULL) out_idx = indices;
    else indices = out_idx;
    double *out_dist = NULL;
    if (return_distance) {
        out_dist = (double *)realloc(distances, count * sizeof(double));
        if (out_dist == NULL) out_dist = distances;
        else distances = out_dist;
    }
    out->indices = indices;
    out->distances = out_dist;
    out->count = count;
    return 0;
}
