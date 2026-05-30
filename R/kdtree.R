#' Build a k-d tree from a numeric matrix
#'
#' Builds a k-d tree over \code{n} points in \code{d} dimensions. Rows of
#' \code{data} are samples; columns are features. Distances are Euclidean
#' in that feature space.
#'
#' @param data numeric matrix (\code{n} rows, \code{d} columns).
#' @return an external pointer of class \code{AirRoute_kdtree}; freed
#'   automatically when garbage-collected.
#' @export
#' @examples
#' data <- matrix(c(0, 0, 1, 0, 100, 0), ncol = 2, byrow = TRUE)
#' tree <- kdtree_build(data)
#' kdtree_query(tree, c(0, 0), k = 2)
kdtree_build <- function(data) {
    ptr <- .Call("kdtree_build_r", data, PACKAGE = "AirRoute")
    structure(ptr, class = "AirRoute_kdtree")
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
    .Call("kdtree_query_r", tree, point, as.integer(k), PACKAGE = "AirRoute")
}
