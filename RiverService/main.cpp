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
    hilltop_data_source otago_source("http://gisdata.orc.govt.nz/Hilltop/data.hts", "Otago Regional Council");
    hilltop_data_source horizons_source("http://TSData.Horizons.govt.nz/data.hts", "Horizons", units(1000,1000), type_dict("Flow", "Stage", "Water Temperature"));
    hilltop_data_source marlborough_source("http://hydro.marlborough.govt.nz/mdc%20data.hts?", "Marlborough District Council");
    hilltop_data_source wellington_source("http://hilltop.gw.govt.nz/data.hts", "Greater Wellington");
    hilltop_data_source gisbourne("http://hilltop.gdc.govt.nz/data.hts", "Gisbourne District Council", units(1, 1), type_dict("Flow", "Water Level", "Water Temperature"));
    hilltop_data_source taranaki("http://extranet.trc.govt.nz/getdata/merged.hts", "Taranaki");

    vector<data_source*> data_sources;
    data_sources.push_back(&otago_source);
    data_sources.push_back(&wellington_source);
    data_sources.push_back(&taranaki);
    data_sources.push_back(&marlborough_source);
    data_sources.push_back(&horizons_source);
    data_sources.push_back(&gisbourne);
    //data_sources.push_back(&niwa_source);

    data_store data(data_sources);
    utility::string_t port = U("5000");
    if (argc == 2) {
        port = utility::conversions::to_string_t(argv[1]);
    }

    server_session server;
    server.create_session(data, port, health);
    return 0;
}