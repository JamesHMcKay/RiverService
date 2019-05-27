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
#include "health_tracker.h"
#include "niwa_data_source.h"
#include "hilltop_data_source.h"

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

    health_tracker health;

    niwa_data_source niwa_source;
    hilltop_data_source otago_source("http://gisdata.orc.govt.nz/Hilltop/Global.hts", "Otago Regional Council");
    hilltop_data_source tasman_source("http://envdata.tasman.govt.nz/data.hts", "Tasman Regional Council");
    hilltop_data_source horizons_source("http://hilltopserver.horizons.govt.nz/data.hts", "Horizons Regional Council", litres_second);
    hilltop_data_source marlborough_source("http://hydro.marlborough.govt.nz/data.hts", "Marlborough Regional Council");

    vector<data_source*> data_sources;
    //data_sources.push_back(&tasman_source);
    data_sources.push_back(&horizons_source);
    //data_sources.push_back(&marlborough_source);
    data_sources.push_back(&niwa_source);

    data_store data(data_sources);
    utility::string_t port = U("5000");
    if (argc == 2) {
        port = utility::conversions::to_string_t(argv[1]);
    }

    server_session server;
    server.create_session(data, port, health);
    return 0;
}