#include "../../src/tcas.h"
#include <cassert>
#include <cstdio>
#include <vector>

int main() {
    const double lats[] = {52.1657, 52.17, 50.0};
    const double lons[] = {20.9671, 20.97, 8.0};

    std::vector<std::size_t> brute;
    std::vector<std::size_t> kd;
    AirRoute::tcas::points_within_radius_km_bruteforce(52.1657, 20.9671, 3, lats, lons, 10.0, brute);
    AirRoute::tcas::points_within_radius_km_kdtree(52.1657, 20.9671, 3, lats, lons, 10.0, kd);
    assert(brute == kd);

    std::vector<std::size_t> knn;
    AirRoute::tcas::knn_bruteforce(52.1657, 20.9671, 3, lats, lons, 2, knn);
    assert(knn.size() == 2);
    assert(knn[0] == 0);
    assert(knn[1] == 1);
    std::printf("tcas OK\n");
    return 0;
}
