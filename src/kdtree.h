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

kdtree_layout kdtree_layout_rowmajor(const double *data, size_t n_samples, size_t n_features);
kdtree_layout kdtree_layout_colmajor(const double *data, size_t n_samples, size_t n_features);

kdtree *kdtree_build(const kdtree_layout *layout);
int kdtree_query(const kdtree *tree, const double *point, size_t k, size_t *out_indices, double *out_distances);
void kdtree_free(kdtree *tree);
