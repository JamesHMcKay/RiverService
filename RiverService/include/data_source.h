#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include <iostream>
#include <locale.h>

#include "feature.h"
#include "observation_type.h"

#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include <string>


using namespace std;

class OrderUpdateQueue {
public:
    bool operator()(
        feature_of_interest* first,
        feature_of_interest* second) {
        return (first->next_update_time) > (second->next_update_time);
    }
};

class data_source {
public:
    string data_source_name;

    virtual uri_builder get_source_uri(string obs_type = "") = 0;

    virtual void get_all_features() = 0;

    virtual void process_flow_response(string flow_res_string, std::map<string, sensor_obs> &result, observable type) = 0;

    virtual void process_rainfall_response(string flow_res_string, std::map<string, sensor_obs> &result, observable type);

    virtual string get_flow_data(utility::string_t feature_id, string lower_time, string type) = 0;

    virtual string get_rainfall_data(utility::string_t feature_id, string lower_time, string type);

    void update_sources();

    void update_feature(feature_of_interest* feature_to_update);

    map<utility::string_t, feature_of_interest*> get_available_features();

    string get_last_updated_time_str() {
        std::time_t time = std::chrono::system_clock::to_time_t(last_updated);
        return ctime(&time);
    }

    std::pair<int, int> create_data_source_summary();

protected:
    bool initiliased;

    string_t _host_url;

    units _source_units;

    type_dict _source_type_dict;

    map<utility::string_t, feature_of_interest*> feature_map;

    std::priority_queue<feature_of_interest*, std::vector<feature_of_interest*>, OrderUpdateQueue> update_queue;

    chrono::system_clock::time_point last_updated = chrono::system_clock::now();

    vector<observation_type> _observation_types;
};

#endif
