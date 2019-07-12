#ifndef HEALTH_TRACKER_H
#define HEALTH_TRACKER_H

#include <iostream>
#include <chrono>
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

    string instance_id;


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


};


#endif