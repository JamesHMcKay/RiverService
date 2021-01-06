#include <iostream>
#include <future>
#include <chrono>
#include <thread>

#include "gtest/gtest.h"
#include "utils.h"
#include "mock_client.hpp"

#include "cpprest/http_client.h"

using namespace web::http::client;
using namespace web::http;

TEST(utils, get_distance_future_time) {
    EXPECT_EQ("2100-06-01T00:00:00.000Z", utils::get_distant_future_time());
}
