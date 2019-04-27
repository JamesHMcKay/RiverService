#ifndef FEATURE_H
#define FEATURE_H

#include <iostream>
#include "lat_lon.h"
#include "sensor_obs.h"
#include "utils.h"
#include "observation_store.h"

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

    string _name;

    utility::string_t _id;

    lat_lon _position;

    vector<sensor_obs> sensor_history;

    string_t _host_url;

    string _data_source_name;

public:
    observation_store obs_store;

    chrono::duration<double> next_update_time = chrono::duration<double>::zero();

    feature_of_interest() {}
    feature_of_interest(string name, utility::string_t id, lat_lon position, string data_source_name) {
        _name = name;
        _position = position;
        _id = id;
        _data_source_name = data_source_name;
    }

    void set_last_checked_for_update_time() {
        last_checked_for_updates = chrono::system_clock::now();
    }

    utility::string_t get_data_source_name() {
        return utility::conversions::to_string_t(_data_source_name);
    }

    utility::string_t get_last_checked_time() {
        return utility::conversions::to_string_t(utils::get_time_utc(last_checked_for_updates));
    }

    //void add_sensor_obs(vector<sensor_obs> observations) {
    //    int num_points = observations.size();
    //    int existing_points = sensor_history.size();
    //    if (num_points > 0 && existing_points > 0) {
    //        cout << "UPDATE DATA RECIEVED" << endl;
    //        update_period = system_clock::now() - last_recieved_data;
    //    }
    //    if (num_points > 0) {
    //        last_recieved_data = system_clock::now();
    //    }
    //    for (int i = 0; i < num_points; i++) {
    //        sensor_history.insert(sensor_history.begin(), observations[i]);
    //    }
    //}

    utility::string_t get_id() {
        return _id;
    }

    lat_lon get_position() {
        return _position;
    }

    string get_name_str() {
        return _name;
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



        next_update_time = current_time_ref + max(absolute_update_period, update_period);// + chrono::seconds(120);

        cout << "setting update time, latest flow = " << latest_point.get_flow() << " penultimate flow = " << penultimate_point.get_flow() << endl;
    }

    void filter_observations(vector<sensor_obs> observations);

    double get_latest_flow() {
        double flow = 0;
        if (obs_store.get_first() != NULL) {
            flow = obs_store.get_first()->value.get_flow();
        }
        return flow;
    }

    string get_lower_time() {
        string lower_time;
        if (obs_store.length == 0) {
            lower_time = utils::ref_time_str();
        }
        else {
            string latest_time = obs_store.get_first()->value.get_time_str();
            int time_str_length = latest_time.size();
            string time_zone = latest_time.substr(time_str_length - 5, time_str_length - 1);
            lower_time = utils::get_time_utc(utils::convert_time_str(latest_time), time_zone);
        }
        return lower_time;
    }
};


#endif