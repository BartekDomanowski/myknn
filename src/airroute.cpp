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
} 