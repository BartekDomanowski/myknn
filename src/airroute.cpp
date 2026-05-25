#include "airroute.h"
#include <cmath>

namespace AirRoute {
    double deg2rad(double deg) {
        return deg*PI/180.0;
    }
    double rad2deg(double rad) {
        return rad*180.0/PI;
    }
    double dist_haversine_km(double lat1, double lon1, double lat2, double lon2) {
        double dlat = deg2rad(lat2-lat1);
        double dlon = deg2rad(lon2-lon1);
        double a = std::sin(dlat/2) * std::sin(dlat/2)+ std::cos(deg2rad(lat1)) * std::cos(deg2rad(lat2)) * std::sin(dlon/2) * std::sin(dlon/2);
        double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
        return EARTH_RADIUS*c;
    }


    double route_length_km(std::size_t n, const double* lats, const double* lons) {
        if (n < 2) return 0.0;
        double total = 0.0;
        for (std::size_t i = 1; i < n; i++) {
            total += dist_haversine_km(lats[i-1], lons[i-1], lats[i], lons[i]);
        }
        return total;
    }

    void route_cumulative_length_km(std::size_t n, const double* lats, const double* lons, double* cum) {
        //whole dist in cum[n-1]
        if (n == 0) return;
        cum[0] = 0.0;
        for (std::size_t i = 1; i < n; i++) {
            cum[i] = cum[i-1] + dist_haversine_km(lats[i-1], lons[i-1], lats[i], lons[i]);
        }
    }



    // deg functions to show dirs on the map
    double initial_bearing_deg(double lat1, double lon1, double lat2, double lon2) {
        const double phi1 = deg2rad(lat1);
        const double phi2 = deg2rad(lat2);
        const double dlon = deg2rad(lon2-lon1);
        const double y = std::sin(dlon) * std::cos(phi2);
        const double x = std::cos(phi1) * std::sin(phi2) - std::sin(phi1) * std::cos(phi2) * std::cos(dlon);
        return rad2deg(std::atan2(y, x)) < 0.0 ? rad2deg(std::atan2(y, x)) + 360.0 : rad2deg(std::atan2(y, x));
    }


    void destination_point(double lat, double lon, double bearing_deg, double distance_km, double* out_lat, double* out_lon) {
        const double brng = deg2rad(bearing_deg);
        const double phi1 = deg2rad(lat);
        const double lam1 = deg2rad(lon);
        const double ang = distance_km / EARTH_RADIUS;
        const double phi2 = std::asin(std::sin(phi1) * std::cos(ang) + std::cos(phi1) * std::sin(ang) * std::cos(brng));
        const double lam2 = lam1 + std::atan2(std::sin(brng) * std::sin(ang) * std::cos(phi1), std::cos(ang) - std::sin(phi1) * std::sin(phi2));
        *out_lat = rad2deg(phi2);
        *out_lon = rad2deg(lam2);
    }

    double angular_distance_rad(double lat1, double lon1, double lat2, double lon2) {
        const double dlat = deg2rad(lat2 - lat1);
        const double dlon = deg2rad(lon2 - lon1);
        const double a = std::sin(dlat / 2) * std::sin(dlat / 2) + std::cos(deg2rad(lat1)) * std::cos(deg2rad(lat2)) * std::sin(dlon / 2) * std::sin(dlon / 2);
        return 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    }

    static double clamp_unit(double x) {
        if (x < -1.0) return -1.0;
        if (x > 1.0) return 1.0;
        return x;
    }

    double cross_track_segment_km(double lat_p, double lon_p, double lat_a, double lon_a, double lat_b, double lon_b) {
        const double d12_km = dist_haversine_km(lat_a, lon_a, lat_b, lon_b); //sphere dist
        if (d12_km < 1e-9) return dist_haversine_km(lat_a, lon_a, lat_p, lon_p);
        const double d13 = angular_distance_rad(lat_a, lon_a, lat_p, lon_p);
        if (d13 < 1e-12) return 0.0;
        const double th13 = deg2rad(initial_bearing_deg(lat_a, lon_a, lat_p, lon_p));
        const double th12 = deg2rad(initial_bearing_deg(lat_a, lon_a, lat_b, lon_b));
        const double xt = std::asin(clamp_unit(std::sin(d13) * std::sin(th13 - th12)));
        const double xt_km = std::fabs(xt) * EARTH_RADIUS;
        const double cos_xt = std::cos(xt);
        if (std::fabs(cos_xt) < 1e-12) return xt_km;
        const double at_km = std::acos(clamp_unit(std::cos(d13) / cos_xt)) * EARTH_RADIUS;
        if (at_km < 0.0) return dist_haversine_km(lat_a, lon_a, lat_p, lon_p);
        if (at_km > d12_km) return dist_haversine_km(lat_b, lon_b, lat_p, lon_p);
        return xt_km;
    }


    // to measure distance from perfection
    double cross_track_route_km(double lat_p, double lon_p, std::size_t n, const double* lats, const double* lons,
                                std::size_t* segment_index) {
        if (n < 2) {
            if (segment_index != nullptr) *segment_index = 0;
            if (n == 1) return dist_haversine_km(lats[0], lons[0], lat_p, lon_p);            
            return 0.0;
        }
        double best = cross_track_segment_km(lat_p, lon_p, lats[0], lons[0], lats[1], lons[1]);
        std::size_t best_seg = 0;
        for (std::size_t i = 1; i + 1 < n; ++i) {
            const double d = cross_track_segment_km(lat_p, lon_p, lats[i], lons[i], lats[i + 1], lons[i + 1]);
            if (d < best) {
                best = d;
                best_seg = i;
            }
        }
        if (segment_index != nullptr) *segment_index = best_seg;
        return best;
    }

    void track_cross_track_km(std::size_t n_track, const double* t_lats, const double* t_lons,std::size_t n_plan, const double* p_lats, const double* p_lons, double* out_km) {
        for (std::size_t i = 0; i < n_track; ++i) 
            out_km[i] = cross_track_route_km(t_lats[i], t_lons[i], n_plan, p_lats, p_lons, nullptr);
    }
}