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

int main() {
    test_dist_haversine_km();
    test_route_length_km();
    test_route_cumulative_length_km();
    std::printf("OK\n");
    return 0;
}
