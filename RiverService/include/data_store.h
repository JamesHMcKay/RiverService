#ifndef DATA_STORE_H
#define DATA_STORE_H

#include "lat_lon.h"
#include "sensor_obs.h"
#include "data_source.h"

#include <iostream>
using namespace std;

class data_store {
    vector<data_source> _data_sources;


public:
    data_store() {}
    data_store(vector<data_source> data_sources) {
        _data_sources = data_sources;
    }

    map<utility::string_t, feature_of_interest> feature_map;

    void update_sources() {
        cout << "updating sources" << endl;
        for (auto source : _data_sources) {
            source.update(feature_map);
        }
    }

};


#endif