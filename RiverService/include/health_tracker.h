#ifndef HEALTH_TRACKER_H
#define HEALTH_TRACKER_H

#include <iostream>
#include <chrono>
#include <data_source.h>
#include <cpprest/json.h>

using namespace web;
using namespace std;
using namespace chrono;

enum application_status {
    okay,
    error
};

class health_tracker {
    chrono::system_clock::time_point start_time;

    int http_requests_made = 0;

    application_status status = okay;

    const char* instance_id;

    vector<data_source*> data_sources;

public:
    health_tracker() {
        start_time = chrono::system_clock::now();
    }

    chrono::duration<double> get_uptime() {
        return chrono::system_clock::now() - start_time;
    }

    string get_status() {
        return "Okay";
    }

    void set_id(const char* id) {
        instance_id = id;
    }

    void set_data_sources(vector<data_source*> &_data_sources) {
        data_sources = _data_sources;
    }

    std::vector<web::json::value> create_data_source_report() {
        std::vector<web::json::value> data_source_reports;
        for (const auto source : data_sources) {
            web::json::value source_report;
            std::pair<int, int> summary = source->create_data_source_summary();
            source_report[U("source_name")] = json::value::string(utility::conversions::to_string_t(source->data_source_name));
            source_report[U("gauges_total")] = json::value(summary.first);
            source_report[U("gauges_active")] = json::value(summary.second);
            data_source_reports.push_back(source_report);
        }

        return data_source_reports;
    }

};


#endif