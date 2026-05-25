#include <Rcpp.h>
#include "airroute.h"

using namespace Rcpp;

static void check_lat_lon(const NumericVector& lats, const NumericVector& lons) {
    if (lats.size() != lons.size()) Rcpp::stop("lats and lons arrays must have the same length");
}



//' @title 
//' distance (Haversine)
//'
//' @description
//' computes the distance between two points on the WGS84 sphere
//' using the haversine formula (with Earth radius 6371.0088 km).
//'
//' @param lat1 latitude of point 1 (degs).
//' @param lon1 longitude of point 1 (degs).
//' @param lat2 latitude of point 2 (degs).
//' @param lon2 longitude of point 2 (degs).
//'
//' @return 
//' distance in km.
//'
//' @examples
//' dist_haversine_km(52.1657, 20.9671, 51.47, -0.4543)  # waw -> lhr ~ 1460-70km
//'
//' @encoding UTF-8
//' @export
// [[Rcpp::export]]
double dist_haversine_km(double lat1, double lon1, double lat2, double lon2) {
    return AirRoute::dist_haversine_km(lat1, lon1, lat2, lon2);
}

//' @title 
//' route length along waypoints
//'
//' @description
//' sums haversine leg distances between consecutive waypoints
//' \code{(lat[i-1], lon[i-1])} to \code{(lat[i], lon[i])}.
//'
//' @param lats numeric vector of latitudes (degs).
//' @param lons numeric vector of longitudes (degs), same length as \code{lats}.
//'
//' @return 
//' total route length in km (0 if fewer than 2 points couse it makes no sense then).
//'
//' @encoding UTF-8
//' @export
// [[Rcpp::export]]
double route_length_km(NumericVector lats, NumericVector lons) {
    check_lat_lon(lats, lons);
    return AirRoute::route_length_km(static_cast<std::size_t>(lats.size()),lats.begin(), lons.begin());
}





//' @title 
//' cumulative route length in array
//'
//' @description
//' returns cumulative haversine distance along a sequence of waypoints.
//' the first element is 0; the last equals \code{\link{route_length_km}(lats, lons)}.
//'
//' @param lats numeric vector of latitudes (degs).
//' @param lons numeric vector of longitudes (degs).
//'
//' @return numeric vector of cumulative distances in km.
//'
//' @encoding UTF-8
//' @export
// [[Rcpp::export]]
NumericVector route_cumulative_length_km(NumericVector lats, NumericVector lons) {
    check_lat_lon(lats, lons);
    NumericVector cum(lats.size());
    AirRoute::route_cumulative_length_km(static_cast<std::size_t>(lats.size()),lats.begin(), lons.begin(), cum.begin());
    return cum;
}


//' @title 
//' initial bearing (deg)
//'
//' @description
//' computes the initial bearing (deg) between two points on the WGS84 sphere.
//'
//' @param lat1 latitude of point 1 (degs).
//' @param lon1 longitude of point 1 (degs).
//' @param lat2 latitude of point 2 (degs).
//' @param lon2 longitude of point 2 (degs).
//'
//' @return initial bearing in degrees, clockwise from north (0--360).
//'
//' @examples
//' initial_bearing_deg(52.1657, 20.9671, 50.0379, 8.5622)  # WAW -> FRA ~ 260 deg
//'
//' @encoding UTF-8
//' @export
// [[Rcpp::export]]
double initial_bearing_deg(double lat1, double lon1, double lat2, double lon2) {
    return AirRoute::initial_bearing_deg(lat1, lon1, lat2, lon2);
}

//' @title
//' destination point along a haversine arc
//'
//' @description
//' given a start point, initial bearing, and distance, returns the
//' destination on the WGS84 sphere (haversine forward geodesic).
//'
//' @param lat latitude of start point (degs).
//' @param lon longitude of start point (degs).
//' @param bearing_deg initial bearing (degs, clockwise from north).
//' @param distance_km distance to travel (km).
//'
//' @return numeric vector of length 2: latitude and longitude (degs).
//'
//' @examples
//' destination_point(52.1657, 20.9671, 90, 100)
//'
//' @encoding UTF-8
//' @export
// [[Rcpp::export]]
NumericVector destination_point(double lat, double lon, double bearing_deg, double distance_km) {
    double out_lat = 0.0;
    double out_lon = 0.0;
    AirRoute::destination_point(lat, lon, bearing_deg, distance_km, &out_lat, &out_lon);
    return NumericVector::create(out_lat, out_lon);
}

//' @title
//' cross-track distance to one route segment
//'
//' @description
//' perpendicular distance from point P to great-circle segment A--B (km).
//'
//' @param lat_p,lon_p point P (degs).
//' @param lat_a,lon_a,lat_b,lon_b segment endpoints A and B (degs).
//' @return cross-track distance in km.
//' @encoding UTF-8
//' @export
// [[Rcpp::export]]
double cross_track_segment_km(double lat_p, double lon_p,
                            double lat_a, double lon_a, double lat_b, double lon_b) {
    return AirRoute::cross_track_segment_km(lat_p, lon_p, lat_a, lon_a, lat_b, lon_b);
}

//' @title
//' cross-track distance to a polyline route
//'
//' @description
//' minimum cross-track distance from P to any segment of the planned route.
//'
//' @param lat_p,lon_p point P (degs).
//' @param plan_lats,plan_lons planned route waypoints (degs).
//' @return minimum cross-track distance in km.
//' @encoding UTF-8
//' @export
// [[Rcpp::export]]
double cross_track_route_km(double lat_p, double lon_p,
                            NumericVector plan_lats, NumericVector plan_lons) {
    check_lat_lon(plan_lats, plan_lons);
    return AirRoute::cross_track_route_km(
        lat_p, lon_p,
        static_cast<std::size_t>(plan_lats.size()),
        plan_lats.begin(), plan_lons.begin(),
        nullptr);
}

//' @title
//' cross-track distance for each track point
//'
//' @description
//' for each point on the track, returns cross-track distance to the
//' nearest segment of the planned route.
//'
//' @param track_lats,track_lons actual track (degs).
//' @param plan_lats,plan_lons planned route (degs).
//' @return numeric vector of cross-track distances in km (same length as track).
//' @encoding UTF-8
//' @export
// [[Rcpp::export]]
NumericVector track_cross_track_km(NumericVector track_lats, NumericVector track_lons,
                                   NumericVector plan_lats, NumericVector plan_lons) {
    check_lat_lon(track_lats, track_lons);
    check_lat_lon(plan_lats, plan_lons);
    NumericVector out(track_lats.size());
    AirRoute::track_cross_track_km(
        static_cast<std::size_t>(track_lats.size()),
        track_lats.begin(), track_lons.begin(),
        static_cast<std::size_t>(plan_lats.size()),
        plan_lats.begin(), plan_lons.begin(),
        out.begin());
    return out;
}
