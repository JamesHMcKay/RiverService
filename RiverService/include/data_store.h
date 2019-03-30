#ifndef DATA_STORE_H
#define DATA_STORE_H

#include "lat_lon.h"
#include "sensor_obs.h"
#include "data_source.h"
#include <chrono>
#include <ctime>

#include <iostream>
using namespace std;

class data_store {
    vector<data_source> _data_sources;
    chrono::system_clock::time_point last_updated = chrono::system_clock::now();

public:
    data_store() {}
    data_store(vector<data_source> data_sources) {
        _data_sources = data_sources;
    }

    map<utility::string_t, feature_of_interest> feature_map;

    void update_sources() {
        cout << "updating sources" << endl;
        last_updated = chrono::system_clock::now();
        for (auto &source : _data_sources) {
            source.update(feature_map);
        }
    }

    string get_last_updated_time_str() {
        std::time_t time = std::chrono::system_clock::to_time_t(last_updated);
        return ctime(&time);
    }
};


#endif