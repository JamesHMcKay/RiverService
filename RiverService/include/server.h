#ifndef SERVER_H
#define SERVER_H

#include <string>
#include "feature.h"

using namespace std;

class server_session {
    string _address;
    vector<sensor_obs> flows;
public:
    server_session() {}
    server_session(string address) {
        _address = address;
    }

    void create_session(map<utility::string_t, feature_of_interest> feature_map, utility::string_t port);
};

#endif