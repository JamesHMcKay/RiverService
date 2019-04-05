#ifndef SERVER_H
#define SERVER_H

#include <string>
#include "feature.h"
#include "data_store.h"
#include "health_tracker.h"

using namespace std;

class server_session {
    string _address;
    vector<sensor_obs> flows;
public:
    server_session() {}
    server_session(string address) {
        _address = address;
    }

    void create_session(data_store &data, utility::string_t port, health_tracker &health);
};

#endif