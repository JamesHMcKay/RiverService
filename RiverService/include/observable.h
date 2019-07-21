#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

using namespace std;

enum unit {
    cumecs,
    metres,
    litres_second,
    millimetres,
    celcius,
    none
};

enum observable: char {
    flow,
    temperature,
    stage_height,
    pH,
    rainfall
};

class units {
    double flow_units;
    double stage_units;
    double rain_units;
public:
    units() {
        flow_units = 1.0;
        stage_units = 1.0;
        rain_units = 1.0;
    }

    units(double flow_units, double stage_units) : flow_units(flow_units), stage_units(stage_units) { rain_units = 1.0; }

    units(double flow_units, double stage_units, double rain_units) : flow_units(flow_units), stage_units(stage_units), rain_units(rain_units) {}

    double get_units(observable obs) {
        double result;
        switch (obs)
        {
        case flow: result = flow_units; break;
        case stage_height: result = stage_units; break;
        case rainfall: result = rain_units; break;
        default: result = 1.0;
        }
        return result;
    }
};

class type_dict {
    string flow_name;
    string stage_name;
    string temp_name;
    string rainfall_name;
public:
    type_dict() {
        flow_name = "Flow";
        stage_name = "Stage";
        temp_name = "Water Temperature";
        rainfall_name = "Rainfall";
    }

    type_dict(string flow_name, string stage_name, string temp_name, string rainfall_name) : flow_name(flow_name), stage_name(stage_name), temp_name(temp_name), rainfall_name(rainfall_name) {}

    type_dict(string flow_name, string stage_name, string temp_name) : flow_name(flow_name), stage_name(stage_name), temp_name(temp_name) {
        rainfall_name = "Rainfall";
    }

    string get_name(observable obs) {
        string result;
        switch (obs)
        {
        case flow: result = flow_name; break;
        case stage_height: result = stage_name; break;
        case temperature: result = temp_name; break;
        case rainfall: result = rainfall_name; break;
        default: result = "";
        }
        return result;
    }
};

#endif