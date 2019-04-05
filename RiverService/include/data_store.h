#ifndef DATA_STORE_H
#define DATA_STORE_H

#include "lat_lon.h"
#include "sensor_obs.h"
#include "data_source.h"
#include <chrono>
#include <ctime>

#include <iostream>
using namespace std;

class OrderUpdateQueue {
public:
    bool operator()(
        feature_of_interest* first,
        feature_of_interest* second) {
        return (first->next_update_time) < (second->next_update_time);
    }
};

class data_store {
    vector<data_source> _data_sources;
    chrono::system_clock::time_point last_updated = chrono::system_clock::now();

    std::priority_queue<feature_of_interest*, std::vector<feature_of_interest*>, OrderUpdateQueue> update_queue;

public:
    data_store() {}
    data_store(vector<data_source> data_sources) {
        _data_sources = data_sources;
    }

    map<utility::string_t, feature_of_interest> feature_map;

    void update_sources();

    void get_available_features();

    string get_last_updated_time_str() {
        std::time_t time = std::chrono::system_clock::to_time_t(last_updated);
        return ctime(&time);
    }
};

#endif