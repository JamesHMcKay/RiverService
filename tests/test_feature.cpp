#include <iostream>

#include "gtest/gtest.h"
#include "utils.h"
#include "feature.h"
#include "mock_client.hpp"

#include "cpprest/http_client.h"

using namespace web::http::client;
using namespace web::http;

TEST(feature, create) {
  feature_of_interest test_feature("test_name", "123", lat_lon(1, 2), "test source name", "test region", "river name");


    EXPECT_EQ(test_feature.get_river_name(), "river name");
    EXPECT_EQ(test_feature.get_region(), "test region");
    EXPECT_EQ(test_feature.get_id(), "123");
    EXPECT_EQ(test_feature.get_position().lat, lat_lon(1, 2).lat);
    EXPECT_EQ(test_feature.get_position().lon, lat_lon(1, 2).lon);
}
