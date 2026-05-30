#include <stddef.h>

typedef struct _node {
    size_t idx;
    struct _node *left;
    struct _node *right;
} node;

typedef struct _kdtree { 
    node* root; 
    size_t n_samples; 
    size_t n_features;
    node *nodes;
    const double *data;
} kdtree;

kdtree* kdtree_build(const double *data, size_t n_samples, size_t n_features);
int kdtree_query(const kdtree *tree, const double *point, size_t k, size_t *out_indices, double *out_distances);
void kdtree_free(kdtree *tree);
