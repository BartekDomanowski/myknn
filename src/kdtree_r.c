#define R_NO_REMAP
#include "kdtree.h"
#include <R.h>
#include <Rinternals.h>
#include <stdlib.h>

static void fin(SEXP x) {
    kdtree *tree = (kdtree *)R_ExternalPtrAddr(x);
    if (tree != NULL) kdtree_free(tree);
}

SEXP kdtree_build_r(SEXP data) {
    if (!Rf_isReal(data))
        Rf_error("'data' must be numeric");
    size_t n = (size_t)Rf_nrows(data);
    size_t p = (size_t)Rf_ncols(data);
    if (n == 0 || p == 0)
        Rf_error("'data' must be a non-empty matrix");

    kdtree_layout layout = kdtree_layout_colmajor(REAL(data), n, p);
    kdtree *tree = kdtree_build(&layout);
    if (tree == NULL)
        Rf_error("failed to build k-d tree");

    SEXP x = PROTECT(R_MakeExternalPtr(tree, R_NilValue, data));
    R_RegisterCFinalizerEx(x, fin, TRUE);
    UNPROTECT(1);
    return x;
}

SEXP kdtree_query_r(SEXP tree, SEXP point, SEXP k) {
    if (TYPEOF(tree) != EXTPTRSXP)
        Rf_error("invalid k-d tree");
    kdtree *t = (kdtree *)R_ExternalPtrAddr(tree);
    if (t == NULL || t->root == NULL)
        Rf_error("invalid k-d tree");
    if (!Rf_isReal(point))
        Rf_error("'point' must be numeric");
    if (Rf_length(point) != (R_xlen_t)t->layout.n_features)
        Rf_error("'point' must have length %zu", t->layout.n_features);
    int kk = Rf_asInteger(k);
    if (kk == NA_INTEGER || kk < 1)
        Rf_error("'k' must be a positive integer");
    if ((size_t)kk > t->layout.n_samples)
        Rf_error("'k' cannot exceed the number of training points");

    SEXP i = PROTECT(Rf_allocVector(INTSXP, kk));
    SEXP d = PROTECT(Rf_allocVector(REALSXP, kk));
    size_t *idx = (size_t *)R_alloc((size_t)kk, sizeof(size_t));
    if (kdtree_query(t, REAL(point), (size_t)kk, idx, REAL(d)) != 0)
        Rf_error("k-d tree query failed");
    for (int t_idx = 0; t_idx < kk; t_idx++)
        INTEGER(i)[t_idx] = (int)idx[t_idx] + 1;

    SEXP ans = PROTECT(Rf_allocVector(VECSXP, 2));
    SET_VECTOR_ELT(ans, 0, i);
    SET_VECTOR_ELT(ans, 1, d);
    SEXP nm = PROTECT(Rf_allocVector(STRSXP, 2));
    SET_STRING_ELT(nm, 0, Rf_mkChar("indices"));
    SET_STRING_ELT(nm, 1, Rf_mkChar("distances"));
    Rf_setAttrib(ans, R_NamesSymbol, nm);
    UNPROTECT(4);
    return ans;
}

SEXP kdtree_query_radius_r(SEXP tree, SEXP point, SEXP r, SEXP return_distance) {
    if (TYPEOF(tree) != EXTPTRSXP)  Rf_error("invalid k-d tree");
    kdtree *t = (kdtree *)R_ExternalPtrAddr(tree);
    if (t == NULL || t->root == NULL)
        Rf_error("invalid k-d tree");
    if (!Rf_isReal(point))
        Rf_error("'point' must be numeric");
    if (Rf_length(point) != (R_xlen_t)t->layout.n_features)
        Rf_error("'point' must have length %zu", t->layout.n_features);
    if (!Rf_isReal(r) || Rf_length(r) != 1)
        Rf_error("'r' must be a numeric scalar");
    double rr = REAL(r)[0];
    if (rr < 0.0)
        Rf_error("'r' must be non-negative");
    int ret_dist = Rf_asLogical(return_distance);
    if (ret_dist == NA_LOGICAL)
        Rf_error("'return_distance' must be TRUE or FALSE");

    size_t n = t->layout.n_samples;
    size_t *idx = (size_t *)R_alloc(n, sizeof(size_t));
    double *dist = ret_dist ? (double *)R_alloc(n, sizeof(double)) : NULL;
    size_t count = 0;
    if (kdtree_query_radius_buf(t, REAL(point), rr, ret_dist, idx, n, &count, dist) != 0)
        Rf_error("k-d tree radius query failed");

    SEXP i = PROTECT(Rf_allocVector(INTSXP, (R_xlen_t)count));
    for (size_t j = 0; j < count; j++)
        INTEGER(i)[j] = (int)idx[j] + 1;

    if (!ret_dist) {
        UNPROTECT(1);
        return i;
    }
    SEXP d = PROTECT(Rf_allocVector(REALSXP, (R_xlen_t)count));
    for (size_t j = 0; j < count; j++)
        REAL(d)[j] = dist[j];
    SEXP ans = PROTECT(Rf_allocVector(VECSXP, 2));
    SET_VECTOR_ELT(ans, 0, i);
    SET_VECTOR_ELT(ans, 1, d);
    SEXP nm = PROTECT(Rf_allocVector(STRSXP, 2));
    SET_STRING_ELT(nm, 0, Rf_mkChar("indices"));
    SET_STRING_ELT(nm, 1, Rf_mkChar("distances"));
    Rf_setAttrib(ans, R_NamesSymbol, nm);
    UNPROTECT(4);
    return ans;
}
