#ifndef LAT_LON_H
#define LAT_LON_H

#include <string>
#include <cpprest/json.h>

using namespace std;
class lat_lon {
public:
    double lat;
    double lon;

    lat_lon() {}

    lat_lon(double lat, double lon): lat(lat), lon(lon) {}

    web::json::value get_lat_lon() {
        web::json::value result;
        result[U("lat")] = web::json::value(lat);
        result[U("lon")] = web::json::value(lon);
        return result;
    };
};

#endif