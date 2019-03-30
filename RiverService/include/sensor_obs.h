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
    utility::string_t _time;
    string _units;
public:
    sensor_obs() {}

    sensor_obs(double value, utility::string_t time, string units) {
        _value = value;
        _units = units;
        _time = time;
    }

    utility::string_t get_time() {
        return _time;
    };

    double get_flow() {
        return _value;
    };

    string print_content() {
        std::ostringstream strs;
        strs << _value;
        std::string str = strs.str();
        //cout << "value " << _value << endl;
        return str;
    }
};

#endif