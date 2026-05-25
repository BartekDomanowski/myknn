#ifndef TCAS_H
#define TCAS_H
#include <cstddef>
#include <vector>
#include "kdtree2d.h"

namespace AirRoute {
namespace tcas {

void points_within_radius_km_bruteforce(double query_lat, double query_lon, std::size_t n, const double* lats, const double* lons, double radius_km, std::vector<std::size_t>& out_indices);

void points_within_radius_km_kdtree(double query_lat, double query_lon, std::size_t n, const double* lats, const double* lons, double radius_km, std::vector<std::size_t>& out_indices);

}  // namespace tcas
}  // namespace AirRoute

#endif
