#ifndef SENSOR_OBS_H
#define SENSOR_OBS_H

#include "cpprest/containerstream.h"
#include "cpprest/filestream.h"
#include "cpprest/http_client.h"
#include "cpprest/json.h"
#include "cpprest/producerconsumerstream.h"
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

using namespace std;

class sensor_obs {
    double _value;
    string _time;
    string _units;
public:
    sensor_obs() {}

    sensor_obs(double value, string time, string units) {
        _value = value;
        _units = units;
        _time = time;
    }

    utility::string_t get_time() {
        return utility::conversions::to_string_t(_time);
    };

    string get_time_str() {
        return _time;
    }

    double get_value() {
        return _value;
    };

    string get_units() {
        return _units;
    }
};

#endif