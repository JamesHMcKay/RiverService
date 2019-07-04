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
public:
    units() {
        flow_units = 1.0;
        stage_units = 1.0;
    }

    units(double flow_units, double stage_units) : flow_units(flow_units), stage_units(stage_units) {}

    double get_units(observable obs) {
        double result;
        switch (obs)
        {
        case flow: result = flow_units; break;
        case stage_height: result = stage_units; break;
        default: result = 1.0;
        }
        return result;
    }
};

class type_dict {
    string flow_name;
    string stage_name;
    string temp_name;
    string rainfall_name = "Rainfall";
public:
    type_dict() {
        flow_name = "Flow";
        stage_name = "Stage";
        temp_name = "Water Temperature";
    }

    type_dict(string flow_name, string stage_name, string temp_name) : flow_name(flow_name), stage_name(stage_name), temp_name(temp_name) {}

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