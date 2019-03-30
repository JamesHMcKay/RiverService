#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include <iostream>
#if defined(WIN32) || defined(_WIN32)
#include <xlocale>
#else
#include <xlocale.h>
#endif

#include "feature.h"

#include "cpprest/containerstream.h"
#include "cpprest/filestream.h"
#include "cpprest/http_client.h"
#include "cpprest/json.h"
#include "cpprest/producerconsumerstream.h"
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "pugixml.hpp"
#include <string>

using namespace std;

class data_source {
    string _host_url;
    bool initiliased = false;
public:
    data_source() {}
    data_source(string host_url) {
        _host_url = host_url;
    }

    void process_feature_response(pugi::xml_node responses, std::vector<feature_of_interest> &result);

    void process_flow_response(pugi::xml_node responses, std::vector<sensor_obs> &result);

    pplx::task<string> get_features();

    pplx::task<string> get_flow_data(utility::string_t feature_id);

    void get_features(map<utility::string_t, feature_of_interest> &features_map);

    void get_flows(feature_of_interest &feature);

    void update(map<utility::string_t, feature_of_interest> &features_map);
};

#endif