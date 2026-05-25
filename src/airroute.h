#ifndef AIRROUTE_H
#define AIRROUTE_H
#include <cstddef>

namespace AirRoute {
    constexpr double EARTH_RADIUS = 6371.0088; // (km)
    constexpr double PI = 3.14159265358979323846; 
    double deg2rad(double deg);
    double rad2deg(double rad);
    double dist_haversine_km(double lat1, double lon1, double lat2, double lon2);
    double route_length_km(std::size_t n, const double* lats, const double* lons);
    void route_cumulative_length_km(std::size_t n, const double* lats, const double* lons, double* cum); //whole array
} // namespace AirRoute

#endif