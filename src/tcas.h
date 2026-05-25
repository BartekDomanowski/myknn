#ifndef TCAS_H
#define TCAS_H

#include <cstddef>
#include <vector>

namespace AirRoute {
namespace tcas {

// Brute-force 2D (lat, lon): indices i with Haversine distance from query <= radius_km.
void points_within_radius_km_bruteforce(
    double query_lat,
    double query_lon,
    std::size_t n,
    const double* lats,
    const double* lons,
    double radius_km,
    std::vector<std::size_t>& out_indices);
}  // namespace tcas
}  // namespace AirRoute

#endif
