#include "tcas.h"
#include "airroute.h"
#include "kdtree2d.h"

#include <cmath>
#include <stdexcept>

namespace AirRoute {
namespace tcas {
    // Equirectangular: x = (lat-lat_ref)*cos(phi_m), y = (lon-lon_ref)
    static void latlon_to_local_m(double ref_lat, double ref_lon, double lat, double lon, double* x_m, double* y_m) {
        const double cos_phi_m = std::cos(deg2rad(0.5 * (ref_lat + lat)));
        *x_m = (lon - ref_lon) * cos_phi_m;
        *y_m = (lat - ref_lat);
    } // d = R * sqrt(x^2 + y^2) but it's not necessary here to compute the distance

    static void build_local_points(double query_lat, double query_lon, std::size_t n, const double* lats, const double* lons, std::vector<kdtree2d::Point2D>& pts) {
        pts.resize(n);
        for (std::size_t i = 0; i < n; ++i) {
            double x = 0.0;
            double y = 0.0;
            latlon_to_local_m(query_lat, query_lon, lats[i], lons[i], &x, &y);
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

        const double radius_m = radius_km * 1000.0;
        const double radius_dist_sq = radius_m * radius_m;
        for (std::size_t i = 0; i < n; ++i) {
            double x = 0.0;
            double y = 0.0;
            latlon_to_local_m(query_lat, query_lon, lats[i], lons[i], &x, &y);
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
        tree.within_radius_m(0.0, 0.0, radius_km * 1000.0, out_indices);
    }

}  // namespace tcas
}  // namespace AirRoute
