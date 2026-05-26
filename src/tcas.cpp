#include "tcas.h"
#include "airroute.h"
#include "kdtree2d.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace AirRoute {
namespace tcas {
    static void latlon_to_local(double ref_lat, double ref_lon, double lat, double lon, double* x, double* y) {
        const double phi_m = 0.5 * (ref_lat + lat);
        *x = deg2rad(lon - ref_lon) * std::cos(deg2rad(phi_m));
        *y = deg2rad(lat - ref_lat);
    }

    static void build_local_points(double query_lat, double query_lon, std::size_t n, const double* lats, const double* lons, std::vector<kdtree2d::Point2D>& pts) {
        pts.resize(n);
        for (std::size_t i = 0; i < n; ++i) {
            double x = 0.0;
            double y = 0.0;
            latlon_to_local(query_lat, query_lon, lats[i], lons[i], &x, &y);
            pts[i].index = i;
            pts[i].x = x;
            pts[i].y = y;
        }
    }

    static double dist_sq(double x1, double y1, double x2, double y2) {
        const double dx = x1 - x2;
        const double dy = y1 - y2;
        return dx * dx + dy * dy;
    }

    void points_within_radius_km_bruteforce(double query_lat, double query_lon, std::size_t n, const double* lats, const double* lons, double radius_km, std::vector<std::size_t>& out_indices) {
        out_indices.clear();
        if (n == 0) return;
        if (lats == nullptr || lons == nullptr) throw std::domain_error("lats and lons must be non-null when n > 0");
        if (radius_km < 0.0) throw std::domain_error("radius_km must be non-negative");

        const double radius_rad = radius_km / EARTH_RADIUS;
        const double radius_dist_sq = radius_rad * radius_rad;
        for (std::size_t i = 0; i < n; ++i) {
            double x = 0.0;
            double y = 0.0;
            latlon_to_local(query_lat, query_lon, lats[i], lons[i], &x, &y);
            if (dist_sq(0.0, 0.0, x, y) <= radius_dist_sq) {
                out_indices.push_back(i);
            }
        }
    }

    void points_within_radius_km_kdtree(double query_lat, double query_lon, std::size_t n, const double* lats, const double* lons, double radius_km, std::vector<std::size_t>& out_indices) {
        out_indices.clear();
        if (n == 0) return;
        if (lats == nullptr || lons == nullptr) throw std::domain_error("lats and lons must be non-null when n > 0");
        if (radius_km < 0.0) throw std::domain_error("radius_km must be non-negative");
        std::vector<kdtree2d::Point2D> pts;
        build_local_points(query_lat, query_lon, n, lats, lons, pts);
        kdtree2d::KDTree2D tree;
        tree.build(pts);
        tree.within_radius_m(0.0, 0.0, radius_km / EARTH_RADIUS, out_indices);
    }

    void knn_bruteforce(double query_lat, double query_lon, std::size_t n, const double* lats, const double* lons, std::size_t k, std::vector<std::size_t>& out_indices) {
        out_indices.clear();
        if (n == 0 || k == 0) return;
        if (k > n) throw std::domain_error("k must be less than or equal to n");
        if (lats == nullptr || lons == nullptr) throw std::domain_error("lats and lons must be non-null when n > 0");

        std::vector<std::pair<double, std::size_t>> by_dist(n);
        for (std::size_t i = 0; i < n; i++) {
            double x = 0.0;
            double y = 0.0;
            latlon_to_local(query_lat, query_lon, lats[i], lons[i], &x, &y);
            by_dist[i] = {x * x + y * y, i};
        }
        std::sort(by_dist.begin(), by_dist.end());
        out_indices.resize(k);
        for (std::size_t j = 0; j < k; ++j) {
            out_indices[j] = by_dist[j].second;
        }
    }

}  // namespace tcas
}  // namespace AirRoute
