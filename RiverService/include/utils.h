#ifndef UTILS_H
#define UTILS_H

#include "feature.h"
#include <iostream>
#if defined(WIN32) || defined(_WIN32)
#include <xlocale>
#else
#include <xlocale.h>
#endif

#include "cpprest/containerstream.h"
#include "cpprest/filestream.h"
#include "cpprest/http_client.h"
#include "cpprest/json.h"
#include "cpprest/producerconsumerstream.h"
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "pugixml.hpp"

using namespace ::pplx;
using namespace utility;
using namespace concurrency::streams;
using namespace std;

using namespace web::http;
using namespace web::http::client;
using namespace web::json;

namespace utils {
}

#endif