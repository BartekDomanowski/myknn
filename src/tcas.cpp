#include "tcas.h"
#include "airroute.h"

namespace AirRoute {
namespace tcas {

void points_within_radius_km_bruteforce(double query_lat, double query_lon,
    std::size_t n, const double* lats, const double* lons, double radius_km,
    std::vector<std::size_t>& out_indices) {
    out_indices.clear();
    if (n==0) return;
    if (lats==nullptr || lons==nullptr) throw std::domain_error("lats and lons must be non-null when n > 0");
    if (radius_km < 0.0) throw std::domain_error("radius_km must be non-negative");

    for (std::size_t i=0; i<n; i++) 
        const double d = dist_haversine_km(query_lat, query_lon, lats[i], lons[i]);
        if (d <= radius_km) out_indices.push_back(i);
    }
}

}  // namespace tcas
}  // namespace AirRoute
