#ifndef FEATURE_H
#define FEATURE_H

#include <iostream>
#include "lat_lon.h"
#include "sensor_obs.h"

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