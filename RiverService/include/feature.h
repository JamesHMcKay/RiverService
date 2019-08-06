#ifndef FEATURE_H
#define FEATURE_H

#include <iostream>
#include "lat_lon.h"
#include "sensor_obs.h"
#include "utils.h"
#include "observation_store.h"
#include "observation_type.h"

#include "cpprest/containerstream.h"
#include "cpprest/filestream.h"
#include "cpprest/http_client.h"
#include "cpprest/json.h"
#include "cpprest/producerconsumerstream.h"
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <iomanip>

#include "pugixml.hpp"

using namespace ::pplx;
using namespace utility;
using namespace concurrency::streams;
using namespace std;
using namespace chrono;

using namespace web::http;
using namespace web::http::client;
using namespace web::json;

class feature_of_interest {
    chrono::duration<double> update_period = chrono::duration<double>::zero();

    system_clock::time_point last_recieved_data;

    system_clock::time_point last_checked_for_updates;

    string_t _name;

    utility::string_t _id;

    lat_lon _position;

    vector<sensor_obs> sensor_history;

    string_t _host_url;

    string_t _data_source_name;

    vector<observation_type> _observation_types;

    string_t _river_name;

    string_t _region;

public:
    int status = 0;

    observation_store obs_store;

    chrono::duration<double> next_update_time = chrono::duration<double>::zero();

    feature_of_interest() {}
    feature_of_interest(string_t name, utility::string_t id, lat_lon position, string_t data_source_name, vector<observation_type> observation_types = {}) {
        _name = name;
        _position = position;
        _id = id;
        _data_source_name = data_source_name;
        _observation_types = observation_types;
        _region = data_source_name;
        _river_name = utility::conversions::to_string_t("undefined");
    }

    feature_of_interest(string_t name, utility::string_t id, lat_lon position, string_t data_source_name, string_t region, string_t river_name) {
        _name = name;
        _position = position;
        _id = id;
        _data_source_name = data_source_name;
        _river_name = river_name;
        _region = region;
    }

    utility::string_t get_river_name() {
        return _river_name;
    }

    utility::string_t get_region() {
        return _region;
    }

    void add_observation_type(observation_type type) {
        _observation_types.push_back(type);
    }

    void set_last_checked_for_update_time() {
        last_checked_for_updates = chrono::system_clock::now();
    }

    utility::string_t get_data_source_name() {
        return _data_source_name;
    }

    utility::string_t get_last_checked_time() {
        return utility::conversions::to_string_t(utils::get_time_utc(last_checked_for_updates));
    }

    utility::string_t get_id() {
        return _id;
    }

    lat_lon get_position() {
        return _position;
    }

    utility::string_t get_name() {
        return utility::conversions::to_string_t(_name);
    }

    vector<sensor_obs> get_sensor_history() {
        return sensor_history;
    }

    void set_update_time(sensor_obs latest_point, sensor_obs penultimate_point) {
        string latest_time = latest_point.get_time_str();

        chrono::system_clock::time_point time1 = utils::convert_time_str(latest_time);

        string penultimate_time = penultimate_point.get_time_str();
        chrono::system_clock::time_point time2 = utils::convert_time_str(penultimate_time);

        chrono::duration<double> absolute_update_period =  time1 - time2;
        chrono::duration<double> latest_time_ref = time1 - utils::convert_time_str(utils::ref_time_str());

        system_clock::time_point current_time = system_clock::now();
        chrono::duration<double> current_time_ref = current_time - utils::convert_time_str(utils::ref_time_str());

        next_update_time = current_time_ref + max(absolute_update_period, update_period);
    }

    void filter_observations(vector<sensor_obs> observations);

    vector<pair<double, observable>> get_latest_values() {
        vector<pair<double, observable>> flow;
        if (obs_store.get_first() != NULL) {
            flow = obs_store.get_first()->value.get_value();
        }
        return flow;
    }

    sensor_obs get_latest_sensor_obs () {
        sensor_obs result;
        vector<observable> avail_types = obs_store.get_available_types();
        vector<pair<double, observable>> values;

        for (auto const& type : avail_types) {
            if (obs_store.get_first(type) != NULL) {
                sensor_obs temp = obs_store.get_first(type)->value;
                values.push_back(make_pair(temp.get_observable(type), type));
            }
        }

        if (obs_store.get_first() != NULL) {
            string last_updated = obs_store.get_first()->value.get_time_str();
            result = sensor_obs(values, last_updated);
        }

        return result;
    }

    string get_lower_time() {
        string lower_time;
        if (obs_store.length == 0) {
            lower_time = utils::ref_time_str();
        }
        else {
            string latest_time = obs_store.get_first()->value.get_time_str();
            int time_str_length = latest_time.size();
            string time_zone = "0";
            string suffix = latest_time.substr(time_str_length - 1, time_str_length - 1);
            if (suffix != "Z") {
                string time_zone = latest_time.substr(time_str_length - 5, time_str_length - 1);
            }
            lower_time = utils::get_time_utc(utils::convert_time_str(latest_time), time_zone);
        }
        return lower_time;
    }

    vector<observation_type> get_observation_types() {
        return _observation_types;
    }
};


#endif