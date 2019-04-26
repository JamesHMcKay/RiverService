cmake_minimum_required(VERSION 3.13)
project(cpprest-example)

set(CMAKE_CXX_STANDARD 14)

message(status "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(src include)
set(HEADER_FILES  include/feature.h include/server.h include/utils.h include/data_source.h include/data_store.h include/lat_lon.h include/observation_store.h include/sensor_obs.h include/niwa_feature.h include/niwa_data_source.h)
set(SRC_FILES  main.cpp src/server.cpp src/utils.cpp src/data_source.cpp src/feature.cpp src/data_store.cpp)


find_package(cpprestsdk REQUIRED)
find_package(pugixml REQUIRED)

set(Boost_INCLUDE_DIR /usr/local/Cellar/boost/1.68.0_1/include)
set(Boost_LIBRARY_DIR /usr/local/Cellar/boost/1.68.0_1/lib)
set(BOOST_ROOT /usr/local/Cellar/boost/1.68.0_1)

find_package(Boost COMPONENTS random system thread filesystem chrono atomic date_time regex REQUIRED)
include_directories(${Boost_INCLUDE_DIR} ${pugixml_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR} ${pugixml_LIBRARY_DIR})



add_executable(cpprest-example ${SRC_FILES} ${HEADER_FILES})
target_link_libraries(cpprest-example PRIVATE cpprestsdk::cpprest ${Boost_LIBRARIES} pugixml)

