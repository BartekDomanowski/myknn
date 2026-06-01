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

typedef struct _kdtree_radius_result {
    size_t *indices;
    double *distances;
    size_t count;
} kdtree_radius_result;

kdtree *kdtree_build(const kdtree_layout *layout);
int kdtree_query(const kdtree *tree, const double *point, size_t k, size_t *out_indices, double *out_distances);
int kdtree_query_radius(const kdtree *tree, const double *point, double r, int return_distance, kdtree_radius_result *out);
void kdtree_radius_result_clear(kdtree_radius_result *out);
void kdtree_free(kdtree *tree);
