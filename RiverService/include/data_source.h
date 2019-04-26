#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include <iostream>
#if defined(WIN32) || defined(_WIN32)
#include <xlocale>
#else
#include <xlocale.h>
#endif

#include "feature.h"
#include "niwa_feature.h"

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
public:
    virtual uri_builder get_source_uri() = 0;

    virtual void process_feature_response(pugi::xml_node responses, map<utility::string_t, feature_of_interest*> &features_map) = 0;

    virtual void get_all_features(map<utility::string_t, feature_of_interest*> &features_map) = 0;

    pplx::task<string> get_features_task();

    void get_available_features(map<utility::string_t, feature_of_interest*> &features_map);

protected:
    bool initiliased;
    string_t _host_url;
};

#endif