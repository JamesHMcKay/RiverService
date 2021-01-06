cmake_minimum_required(VERSION 3.8)
project(crest)

find_package(Boost REQUIRED COMPONENTS system thread log program_options chrono)
find_package(Threads REQUIRED)

find_package(OpenSSL)
find_package(ZLIB)
find_package(cpprestsdk REQUIRED)

set(pugixml_LIBRARY_DIR ../pugixml-1.9/build/)
set(pugixml_INCLUDE_DIR ../pugixml-1.9/src/)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message("-- configuring clang options")
    set(CMAKE_CXX_FLAGS "-arch x86_64 -std=c++14 -stdlib=libc++ -DBOOST_LOG_DYN_LINK -Wno-deprecated-declarations")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message("-- configuring gcc options")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -DBOOST_LOG_DYN_LINK")
endif()
set(CPPFLAGS -stdlib=libc++ LDFLAGS -lcpprest -lssl -lcrypto -lboost_system -lboost_thread-mt -lboost_chrono-mt -lpthread -pthread -lz)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -Wall ")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3 ")
set(CMAKE_CXX_STANDARD 14)
set(
  SOURCE_FILES
  RiverService/include/feature.h
  RiverService/include/server.h
  RiverService/include/utils.h
  RiverService/include/data_source.h
  RiverService/include/data_store.h
  RiverService/include/lat_lon.h
  RiverService/include/observation_store.h
  RiverService/include/sensor_obs.h
  RiverService/src/server.cpp
  RiverService/src/utils.cpp
  RiverService/src/data_source.cpp
  RiverService/src/data_store.cpp
  RiverService/src/feature.cpp
  RiverService/include/health_tracker.h
  RiverService/include/niwa_data_source.h
  RiverService/include/hilltop_data_source.h
  RiverService/include/ecan_data_source.h
  RiverService/include/bop_data_source.h
  RiverService/include/kiwis_data_source.h
  RiverService/include/observable.h
  RiverService/include/observation_type.h
)
add_executable(crest RiverService/main.cpp ${SOURCE_FILES})

set(APP_INCLUDE_DIR "RiverService/include")
set(HEADER_SEARCH_PATHS ${CPPRESTSDK_INCLUDE_DIR} ${APP_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIRS} pugixml)

set(PUGIXML_LIBRARY "${PROJECT_SOURCE_DIR}/../pugixml-1.9/build/libpugixml.a")
set(OPENSSL_LIBS "${OPENSSL_LIBRARIES}")
set(LIBRARIES_SEARCH_PATHS ${PUGIXML_LIBRARY} ${OPENSSL_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

include_directories(${HEADER_SEARCH_PATHS} ${pugixml_INCLUDE_DIR})
link_directories(${pugixml_LIBRARY_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE cpprestsdk::cpprest ${LIBRARIES_SEARCH_PATHS})

# Build tests
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

add_executable(run_tests
  tests/main.cpp
  tests/test_utils.cpp
  ${SOURCE_FILES}
)

#include_directories(${HEADER_SEARCH_PATHS} ${pugixml_INCLUDE_DIR})

target_include_directories (
    run_tests
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks/
    ${HEADER_SEARCH_PATHS}
    ${pugixml_INCLUDE_DIR}
)

target_link_libraries(run_tests PRIVATE gtest_main gmock_main)

enable_testing()
add_test(NAME run_tests COMMAND run_tests)

target_link_libraries(run_tests PRIVATE cpprestsdk::cpprest ${LIBRARIES_SEARCH_PATHS})