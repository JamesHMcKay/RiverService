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
    pH
};

#endif