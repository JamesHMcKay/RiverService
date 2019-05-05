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
#include "observable.h"

using namespace std;

class sensor_obs {
    vector<pair<double, observable>> _value;
    string _time;
public:
    sensor_obs() {}

    sensor_obs(vector<pair<double, observable>> value, string time) {
        _value = value;
        _time = time;
    }

    sensor_obs(double value, string time, observable type) {
        _value = { make_pair(value, type) };
        _time = time;
    }

    utility::string_t get_time() {
        return utility::conversions::to_string_t(_time);
    };

    string get_time_str() {
        return _time;
    }

    sensor_obs operator+(sensor_obs& s) {
        if (_time != s.get_time_str()) {
            wcout << "Attempted to add sensor observations at different time values" << endl;
            return *this;
        }
        vector<pair<double, observable>> values = s.get_value();
        vector<pair<double, observable>> this_value = _value;
        for (auto &value : values) {
            this_value.push_back(value);
        }
        sensor_obs result(this_value, _time);
        return result;
    }

    vector<pair<double, observable>> get_value() {
        return _value;
    }

    double get_observable(observable type) {
        for (auto &entry : _value) {
            if (entry.second == type) {
                return entry.first;
            }
        }
        return 0;
    }
};

#endif