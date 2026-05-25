#include "../../src/airroute.h"
#include <cassert>
#include <cmath>
#include <cstdio>

static void test_dist_haversine_km() {
    const double d = AirRoute::dist_haversine_km(52.1657, 20.9671, 51.47, -0.4543);
    assert(d > 1430.0 && d < 1490.0);
}

static void test_route_length_km() {
    const double lats[] = {52.1657, 50.0379, 51.47};
    const double lons[] = {20.9671, 8.5622, -0.4543};
    const double total = AirRoute::route_length_km(3, lats, lons);
    assert(total > 0.0);
}

static void test_route_cumulative_length_km() {
    const double lats[] = {52.1657, 50.0379, 51.47};
    const double lons[] = {20.9671, 8.5622, -0.4543};
    double cum[3];
    AirRoute::route_cumulative_length_km(3, lats, lons, cum);
    const double total = AirRoute::route_length_km(3, lats, lons);
    assert(std::fabs(cum[2] - total) < 1e-6);
}


static void test_initial_bearing_deg() {
    // waw -> fra: ~260 deg (WSW)
    const double tmp = AirRoute::initial_bearing_deg(52.1657, 20.9671, 50.0379, 8.5622);
    assert(tmp > 258.0 && tmp < 262.0);
}

static void test_destination_point() {
    double out_lat = 0.0;
    double out_lon = 0.0;
    AirRoute::destination_point(52.1657, 20.9671, 90.0, 100.0, &out_lat, &out_lon);
    const double tmp = AirRoute::dist_haversine_km(52.1657, 20.9671, out_lat, out_lon);
    assert(tmp > 95.0 && tmp < 105.0);
}

int main() {
    test_dist_haversine_km();
    test_route_length_km();
    test_route_cumulative_length_km();
    test_initial_bearing_deg();
    test_destination_point();
    std::printf("OK\n");
    return 0;
}
