expect_knn <- function(tree, point, k, want_idx, want_dist, tol = 1e-9) {
    res <- kdtree_query(tree, point, k)
    expect_equal(res$indices, want_idx)
    expect_equal(res$distances, want_dist, tolerance = tol)
}

# 1D: points x = 0, 10, 20; query x = 1 -> nearest index 1 
data_1d <- matrix(c(0, 10, 20), ncol = 1)
tree_1d <- kdtree_build(data_1d)
expect_knn(tree_1d, c(1), 1L, 1L, 1)

# query on training point (0,0); points (0,0), (3,4), (100,0)
data_train <- matrix(c(0, 0, 3, 4, 100, 0), ncol = 2, byrow = TRUE)
tree_train <- kdtree_build(data_train)
expect_knn(tree_train, c(0, 0), 1L, 1L, 0)

# two closest to (0,0): (0,0) and (1,0) on x-axis
data_2d <- matrix(c(0, 0, 1, 0, 100, 0), ncol = 2, byrow = TRUE)
tree_2d <- kdtree_build(data_2d)
expect_knn(tree_2d, c(0, 0), 2L, c(1L, 2L), c(0, 1))
