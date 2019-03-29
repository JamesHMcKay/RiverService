#ifndef FEATURE_H
#define FEATURE_H

#include <iostream>

#include "cpprest/containerstream.h"
#include "cpprest/filestream.h"
#include "cpprest/http_client.h"
#include "cpprest/json.h"
#include "cpprest/producerconsumerstream.h"
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "pugixml.hpp"

using namespace ::pplx;
using namespace utility;
using namespace concurrency::streams;
using namespace std;

using namespace web::http;
using namespace web::http::client;
using namespace web::json;


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

class feature_of_interest {
    string _name;
    utility::string_t _id;
    lat_lon _position;
    vector<sensor_obs> sensor_history;

public:
    feature_of_interest() {}
    feature_of_interest(string name, utility::string_t id, lat_lon position) {
        _name = name;
        _position = position;
        _id = id;
    }

    void add_sensor_obs(vector<sensor_obs> observations) {
        for (unsigned int i = 0; i < observations.size(); i++) {
            sensor_history.push_back(observations[i]);
        }
    }

    utility::string_t get_id() {
        return _id;
    }

    utility::string_t get_name() {
        return utility::conversions::to_string_t(_name);
    }

    vector<sensor_obs> get_sensor_history() {
        return sensor_history;
    }

    void print_content() {
        cout << "name = " << _name << " lattitude = " << _position.print_content() << endl;
    }

};


#endif