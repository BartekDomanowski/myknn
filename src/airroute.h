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
    // deg functions to show dirs on the map
    double initial_bearing_deg(double lat1, double lon1, double lat2, double lon2);
    void destination_point(double lat, double lon, double bearing_deg, double distance_km, double* out_lat, double* out_lon);
    // to measure distance from perfection
    double angular_distance_rad(double lat1, double lon1, double lat2, double lon2);
    double cross_track_segment_km(double lat_p, double lon_p, double lat_a, double lon_a, double lat_b, double lon_b);
    double cross_track_route_km(double lat_p, double lon_p, std::size_t n, const double* lats, const double* lons,std::size_t* segment_index);
    void track_cross_track_km(std::size_t n_track, const double* t_lats, const double* t_lons, std::size_t n_plan, const double* p_lats, const double* p_lons, double* out_km);
} // namespace AirRoute

#endif