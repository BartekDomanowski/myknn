#include <stddef.h>

typedef struct _node {
    size_t idx;
    struct _node *left;
    struct _node *right;
} node;

typedef struct {
    const double *data;
    size_t n_samples;
    size_t n_features;
    size_t sample_stride;
    size_t feature_stride;
} kdtree_layout;

typedef struct _kdtree {
    kdtree_layout layout;
    node *root;
    node *nodes;
} kdtree;

//helpers for diff layouts

static inline double kdtree_get(const kdtree_layout *layout, size_t sample, size_t feature) {
    return layout->data[sample * layout->sample_stride + feature * layout->feature_stride];
}

static inline kdtree_layout kdtree_layout_rowmajor(const double *data, size_t n_samples, size_t n_features) {
    kdtree_layout layout = {data, n_samples, n_features, n_features, 1};
    return layout;
}

static inline kdtree_layout kdtree_layout_colmajor(const double *data, size_t n_samples, size_t n_features) {
    kdtree_layout layout = {data, n_samples, n_features, 1, n_samples};
    return layout;
}

kdtree *kdtree_build(const kdtree_layout *layout);
int kdtree_query(const kdtree *tree, const double *point, size_t k, size_t *out_indices, double *out_distances);
void kdtree_free(kdtree *tree);
