#if defined(WIN32) || defined(_WIN32)
#include <xlocale>
#else
#include <xlocale.h>
#endif
#include "server.h"
#include "pugixml.hpp"
#include "feature.h"
#include "utils.h"
#include "data_source.h"
#include "data_store.h"

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <future>

#include <thread>
#include <chrono>

using namespace std;

using namespace web::http;
using namespace web::http::client;
using namespace web::json;

using namespace utils;

int main(int argc, char *argv[]) {

    data_source niwa_source("https://hydro-sos.niwa.co.nz/");
    vector<data_source> data_sources;
    data_sources.push_back(niwa_source);

    data_store data(data_sources);

    //map<utility::string_t, feature_of_interest> features_map;
    utility::string_t port = U("5000");
    if (argc == 2) {
        port = utility::conversions::to_string_t(argv[1]);
    }

    server_session server;
    //std::future<void> result = std::async(get_features, std::ref(features_map));

    server.create_session(data, port);
    //result.get();
    // for (auto const& x : features_map) {
    //     get_flows(features_map[x.first]);
    // }
    
    return 0;
}