#ifndef LAT_LON_H
#define LAT_LON_H

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class lat_lon {
    double _lat;
    double _lon;
public:
    lat_lon() {}

    lat_lon(double lat, double lon) {
        _lat = lat;
        _lon = lon;
    }

    string print_content() {
        std::ostringstream strs;
        strs << _lat;
        std::string str = strs.str();
        return str;
    }
};

#endif