#ifndef LAT_LON_H
#define LAT_LON_H

#include <iostream>
#include <sstream>
#include <string>

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <fstream>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

using namespace std;
class lat_lon {
public:
    double lat;
    double lon;

    lat_lon() {}

    lat_lon(double lat, double lon): lat(lat), lon(lon) {}

    web::json::value get_lat_lon() {
        web::json::value result;
        result[U("lat")] = json::value(lat);
        result[U("lon")] = json::value(lon);
        return result;
    };
};

#endif