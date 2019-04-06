#ifndef OBSERVATION_STORE_H
#define OBSERVATION_STORE_H

#include <iostream>
#include <chrono>
#include "lat_lon.h"
#include "sensor_obs.h"
#include "utils.h"

using namespace std;
using namespace chrono;

template <class T> class observation {
public:
    observation() {}
    observation(T value) : value(value) {}

    T value;
    observation* prev = NULL;
    observation* next = NULL;
};


class observation_store {
    observation<sensor_obs>* first = NULL;
    observation<sensor_obs>* last = NULL;

    void set_up(observation<sensor_obs>* obs_input) {
        first = obs_input;
        first->prev = last;
    }

public:
    observation_store() {}
    observation_store(sensor_obs obs) {
        observation<sensor_obs>* obs_input = new observation<sensor_obs>(obs);
        set_up(obs_input);
    }

    int length = 0;

    observation<sensor_obs>* get_first() {
        return first;
    }

    observation<sensor_obs>* get_last() {
        return last;
    }

    void add_to_top(sensor_obs obs) {
        observation<sensor_obs>* obs_input = new observation<sensor_obs>(obs);
        if (first == NULL) {
            set_up(obs_input);
        } else {
            if (last == NULL) {
                last = first;
                last->next = first;
                last->prev = NULL;
            }
            obs_input->prev = first;
            first->next = obs_input;
            first = obs_input;
        }
        length++;
    }

    // move this logic directly into the server
    vector<sensor_obs> get_as_vector() {
        vector<sensor_obs> result;
        observation<sensor_obs>* node = first;
        while (node->prev != NULL) {
            result.push_back(node->value);
            node = node->prev;
        }
        result.push_back(node->value);
        return result;
    }

    bool will_remove_point(sensor_obs first, chrono::duration<double> time_span, system_clock::time_point latest_time) {
        system_clock::time_point obs_time = utils::get_time_point_utc(first.get_time_str());
        if (latest_time - obs_time > time_span) {
            return true;
        }
        return false;
    }

    void remove_old_points(chrono::duration<double> time_span) {
        if (first != NULL) {
            system_clock::time_point latest_time = utils::get_time_point_utc(first->value.get_time_str());
            vector<sensor_obs> result;
            observation<sensor_obs>* node = last;
            while (node->next != NULL && will_remove_point(node->value, time_span, latest_time)) {
                last = node->next;
                last->prev = NULL;
                delete node;
                node = last;
                length--;
            }
        }
    }
};


#endif