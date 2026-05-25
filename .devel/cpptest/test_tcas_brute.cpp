#include "../../src/tcas.h"
#include "../../src/airroute.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

static bool contains(const std::vector<std::size_t>& v, std::size_t i) {
    return std::find(v.begin(), v.end(), i) != v.end();
}

static void test_empty_set() {
    const double lats[] = {52.0};
    const double lons[] = {21.0};
    std::vector<std::size_t> out;
    AirRoute::tcas::points_within_radius_km_bruteforce(52.0, 21.0, 0, lats, lons, 100.0, out);
    assert(out.empty());
}

static void test_three_points() {
    // waw chopin airport
    const double q_lat = 52.1657;
    const double q_lon = 20.9671;
    //waw, waw city centre, fra 
    const double lats[] = {52.1657, 52.55, 50.0379};
    const double lons[] = {20.9671, 21.0, 8.5622};
    const std::size_t n = 3;
    const double d1 = AirRoute::dist_haversine_km(q_lat, q_lon, lats[1], lons[1]); // waw airport to city centre
    const double d2 = AirRoute::dist_haversine_km(q_lat, q_lon, lats[2], lons[2]); // waw -> frankfurt
    std::vector<std::size_t> out;
    AirRoute::tcas::points_within_radius_km_bruteforce(q_lat, q_lon, n, lats, lons, 100.0, out);
    assert(contains(out, 0));
    assert(d1 <= 100.0 ? contains(out, 1) : !contains(out, 1));
    assert(!contains(out, 2));
    assert(d2 > 100.0);
    out.clear();
    AirRoute::tcas::points_within_radius_km_bruteforce(q_lat, q_lon, n, lats, lons, 1e6, out);
    assert(out.size() == n);
}

static void test_small_radius() {
    const double lats[] = {50.0379, 51.47};
    const double lons[] = {8.5622, -0.4543};
    std::vector<std::size_t> out;
    AirRoute::tcas::points_within_radius_km_bruteforce(52.1657, 20.9671, 2, lats, lons, 1.0, out);
    assert(out.empty());
}


static void test_zero_distance() {
    const double lats[] = {52.1, 52.2};
    const double lons[] = {21.0, 21.1};
    std::vector<std::size_t> out;
    AirRoute::tcas::points_within_radius_km_bruteforce(52.1, 21.0, 2, lats, lons, 0.0, out);
    assert(out.size() == 1 && out[0] == 0);
}

int main() {
    test_empty_set();
    test_three_points();
    test_small_radius();
    test_zero_distance();
    std::printf("tcas brute force OK\n");
    return 0;
}
