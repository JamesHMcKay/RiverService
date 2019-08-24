#if defined(WIN32) || defined(_WIN32)
#include <xlocale>
#else
#include <xlocale.h>
#endif
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <fstream>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

#include <iostream>
#include <map>
#include <set>
#include <string>
#include "server.h"
#include <thread>

using namespace std;

#define TRACE(msg)            wcout << msg
#define TRACE_ACTION(a, k, v) ucout << a << L" (" << k << L", " << v << L")\n"

map<utility::string_t, utility::string_t> dictionary;

const std::function<void(http_request)> handle_get_wrapped(health_tracker &health) {
    return ([&health](http_request request) {
        //TRACE(L"\nhandle GET\n");
        auto uri = request.absolute_uri();
        auto v_path_components = web::uri::split_path(web::uri::decode(uri.path()));
        auto answer = json::value::object();

        if (uri.path() == utility::conversions::to_string_t("/status")) {
            string_t status = utility::conversions::to_string_t(health.get_status());
            answer[U("status")] = json::value::string(status);
            chrono::duration<double> up_time_duration = health.get_uptime();
            auto up_time = std::chrono::duration_cast<std::chrono::seconds>(up_time_duration).count();
            answer[U("up_time_seconds")] = json::value(up_time);

            answer[U("data_sources")] = web::json::value::array(health.create_data_source_report());

            request.reply(status_codes::OK, answer);
        }
        else {
            request.reply(status_codes::OK, "This is the river service");
        }

    });
}

void handle_get(http_request request)
{
    //TRACE(L"\nhandle GET\n");

    auto answer = json::value::object();

    for (auto const & p : dictionary)
    {
        answer[p.first] = json::value::string(p.second);
    }

    request.reply(status_codes::OK, answer);
}

void handle_request(
    http_request request,
    function<void(json::value const &, json::value &)> action)
{
    auto answer = json::value::object();

    request
        .extract_json()
        .then([&answer, &action](pplx::task<json::value> task) {
        try
        {
            auto const & jvalue = task.get();
            //display_json(jvalue, L"R: ");

            if (!jvalue.is_null())
            {
                action(jvalue, answer);
            }
        }
        catch (http_exception const & e) {
            wcout << utility::conversions::to_string_t(e.what()).c_str() << endl;
        }
    })
        .wait();

    request.reply(status_codes::OK, answer);
}

json::value get_available_features(data_store &data, vector<string_t> requested_types) {
    map<utility::string_t, feature_of_interest*> feature_map = data.feature_map;
    web::json::value response;
    std::vector<web::json::value> features;
    for (auto const & item : feature_map) {
        bool passed_filter = false;
        feature_of_interest* feature = item.second;
        web::json::value feature_item;
        feature_item[U("id")] = json::value(feature->get_id());
        feature_item[U("name")] = json::value::string(feature->get_name());

        lat_lon position = feature->get_position();
        feature_item[U("location")] = position.get_lat_lon();
        feature_item[U("data_source")] = json::value::string(feature->get_data_source_name());
        feature_item[U("region")] = json::value::string(feature->get_region());
        feature_item[U("river_name")] = json::value::string(feature->get_river_name());

        vector<observation_type> observation_types = feature->get_observation_types();
        std::vector<web::json::value> obs_types;
        sensor_obs latest_values = feature->get_latest_sensor_obs();
        feature_item[U("last_updated")] = json::value::string(latest_values.get_time());

        vector<observable> available_types = latest_values.get_available_types();

        for (unsigned int i = 0; i < observation_types.size(); i++) {
            web::json::value obs_item;
            auto& type = observation_types[i];
            if (std::find(available_types.begin(), available_types.end(), type.get_obs_type()) != available_types.end()) {
                obs_item[U("type")] = json::value(type.get_type());
                obs_item[U("units")] = json::value(type.get_units());
                obs_item[U("latest_value")] = json::value(latest_values.get_observable(type.get_obs_type()));
                obs_types.push_back(obs_item);
                for (auto &requested_type : requested_types) {
                    if (requested_type == type.get_type()) {
                        passed_filter = true;
                    }
                }
            }
        }
        if (passed_filter) {
            feature_item[U("observables")] = web::json::value::array(obs_types);
            features.push_back(feature_item);
        }
    }
    response[U("features")] = web::json::value::array(features);

    return response;
}

json::value get_flow_response(data_store &data, json::value ids) {
    json::value answer;
    for (auto const & e : ids.as_array())
    {
        if (e.is_string())
        {
            auto key = e.as_string();
            auto pos = data.feature_map.find(key);

            if (pos == data.feature_map.end())
            {
                answer[U("response")] = json::value::string(U("no feature with that id"));
            }
            else
            {
                feature_of_interest* feature = pos->second;
                vector<sensor_obs> flow_history = feature->obs_store.get_as_vector();

                answer[U("id")] = json::value::string(feature->get_id());
                answer[U("name")] = json::value::string(feature->get_name());
                answer[U("last_updated")] = json::value::string(feature->get_last_checked_time());

                std::vector<web::json::value> flowOut;
                vector<observation_type> observation_types = feature->get_observation_types();

                for (unsigned int i = 0; i < flow_history.size(); i++) {
                    sensor_obs item = flow_history[i];
                    web::json::value history_item;
                    vector<pair<double, observable>> values = item.get_value();
                    // note that this is dangerous, if length of values and obs types are different will get wrong units and types on values
                    for (unsigned int i = 0; i < values.size(); i++) {
                        string_t type = utility::conversions::to_string_t(utils::observable_to_string(values[i].second));
                        history_item[type] = json::value(values[i].first);
                    }

                    history_item[U("time")] = json::value::string(item.get_time());
                    flowOut.push_back(history_item);
                }
                answer[U("flows")] = web::json::value::array(flowOut);
            }
        }
    }
    return answer;
}

const std::function<void(http_request)> handle_post_wrapped(data_store &data) {
    return ([&data](http_request request) {
        //TRACE("\nhandle POST\n");

        handle_request(
            request,
            [&data, &request](json::value const & jvalue, json::value & answer) {
            json::value action = jvalue.at(U("action"));
            string_t action_str = action.as_string();

            if (action_str == utility::conversions::to_string_t("get_flows")) {
                json::value ids = jvalue.at(U("id"));
                answer = get_flow_response(data, ids);
            }

            if (action_str == utility::conversions::to_string_t("get_features")) {
                json::value filters = jvalue.at(U("filters"));
                vector<string_t> requested_types;

                for (auto const & e : filters.as_array()) {
                    if (e.is_string()) {
                        requested_types.push_back(e.as_string());
                    }
                }
                answer = get_available_features(data, requested_types);
            }

            http_response response(status_codes::OK);
            response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
            response.set_body(answer);
            request.reply(response);
        });
    });
}

void handle_options(http_request request)
{
    http_response response(status_codes::OK);
    response.headers().add(U("Allow"), U("GET, POST, OPTIONS"));
    response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
    response.headers().add(U("Access-Control-Allow-Methods"), U("GET, POST, OPTIONS"));
    response.headers().add(U("Access-Control-Allow-Headers"), U("authorization, Access-Control-Allow-Origin, Access-Control-Allow-Headers, Origin,Accept, X-Requested-With, Content-Type, Access-Control-Request-Method, Access-Control-Request-Headers"));

    request.reply(response);
}

void server_session::create_session(data_store &data, utility::string_t port, health_tracker &health) {
#if defined(WIN32) || defined(_WIN32)
    string host = "http://localhost:";
#else
    string host = "http://0.0.0.0:";
#endif
    utility::string_t address = utility::conversions::to_string_t(host) + port;

    http_listener listener(address);
    wcout << "listening on " << port.c_str() << endl;
    listener.support(methods::GET, handle_get_wrapped(health));
    listener.support(methods::POST, handle_post_wrapped(data));
    listener.support(methods::OPTIONS, handle_options);

    try
    {
        listener
            .open()
            .then([&listener]() {TRACE(L"\nstarting to listen\n"); })
            .wait();
        data.set_up_feature_map();
        while (true) {
            // return a value that is the time until the next update is required
            // for use in the sleep function
            data.update_sources();
            std::this_thread::sleep_for(std::chrono::seconds(20));
        }
    }
    catch (exception const & e) {
        wcout << utility::conversions::to_string_t(e.what()).c_str() << endl;
    }

}