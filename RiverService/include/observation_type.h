#ifndef OBSERVATION_TYPE_H
#define OBSERVATION_TYPE_H

#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "observable.h"

using namespace std;

class observation_type {
    observable type;
    unit units;
    string source_name;
public:
    observation_type() {}

    observation_type(observable type, unit units, string source_name) : type(type), units(units), source_name(source_name) {}

    string get_source_name() {
        return source_name;
    }

    string_t get_type() {
        string result;
        switch (type)
        {
        case flow: result = "flow"; break;
        case stage_height: result = "stage_height"; break;
        case temperature: result = "temperature"; break;
        case pH: result = "pH"; break;
        case rainfall: result = "rainfall"; break;
        default: result = "value";
        }
        return utility::conversions::to_string_t(result);
    }

    string_t get_units() {
        string result;
        switch (units)
        {
        case cumecs: result = "cumecs"; break;
        case metres: result = "metres"; break;
        case litres_second: result = "litres_second"; break;
        case celcius : result = "celcius"; break;
        default: result = "value";
        }
        return utility::conversions::to_string_t(result);
    }

    observable get_obs_type() {
        return type;
    }
};

#endif