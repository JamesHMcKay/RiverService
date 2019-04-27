#include <functional>
#include <queue>
#include <vector>
#include <iostream>

#include "data_store.h"

using namespace std;
using namespace chrono;

void data_store::set_up_feature_map() {
    for (auto &source : _data_sources) {
        map<utility::string_t, feature_of_interest*> feature_map_temp = source->get_available_features();
        for (auto &entry : feature_map_temp) {
            feature_map[entry.first] = entry.second;
        }
    }
}

void data_store::update_sources() {
    for (auto &source : _data_sources) {
        source->update_sources();
    }
}
