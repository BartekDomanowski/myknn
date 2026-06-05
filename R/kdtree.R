#' Build a k-d tree from a numeric matrix
#'
#' Builds a k-d tree over \code{n} points in \code{d} dimensions. Rows of
#' \code{data} are samples; columns are features. Distances are Euclidean
#' in that feature space.
#'
#' @param data numeric matrix (\code{n} rows, \code{d} columns).
#' @return an external pointer of class \code{myknn_kdtree}; freed
#'   automatically when garbage-collected.
#' @export
#' @examples
#' data <- matrix(c(0, 0, 1, 0, 100, 0), ncol = 2, byrow = TRUE)
#' tree <- kdtree_build(data)
#' kdtree_query(tree, c(0, 0), k = 2)
kdtree_build <- function(data) {
    ptr <- .Call("kdtree_build_r", data, PACKAGE = "myknn")
    structure(ptr, class = "myknn_kdtree")
}

#' Query k nearest neighbours
#'
#' @param tree object returned by [kdtree_build].
#' @param point numeric vector of length \code{ncol(data)} used at build time.
#' @param k number of neighbours (positive integer, at most \code{nrow(data)}).
#' @return a list with \code{indices} (1-based row indices into \code{data})
#'   and \code{distances} (Euclidean, same units as \code{data}).
#' @export
#' @examples
#' data <- matrix(c(0, 0, 1, 0, 100, 0), ncol = 2, byrow = TRUE)
#' tree <- kdtree_build(data)
#' kdtree_query(tree, c(0, 0), k = 2)
kdtree_query <- function(tree, point, k) {
    .Call("kdtree_query_r", tree, point, as.integer(k), PACKAGE = "myknn")
}

#' Query neighbours within a radius
#'
#' @param tree object returned by [kdtree_build].
#' @param point numeric vector of length \code{ncol(data)} used at build time.
#' @param r non-negative radius (Euclidean distance).
#' @param return_distance if \code{TRUE}, return a list with
#'   \code{indices} (1-based) and \code{distances}; if \code{FALSE}, return
#'   only the index vector.
#' @return integer vector of 1-based row indices, or a named list when
#'   \code{return_distance = TRUE}.
#' @export
#' @examples
#' data <- matrix(c(0, 0, 1, 0, 100, 0), ncol = 2, byrow = TRUE)
#' tree <- kdtree_build(data)
#' kdtree_query_radius(tree, c(0, 0), r = 1.5)
kdtree_query_radius <- function(tree, point, r, return_distance = FALSE) {
    .Call("kdtree_query_radius_r", tree, point, as.double(r), return_distance, PACKAGE = "myknn")
}
