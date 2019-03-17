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

using namespace std;

#define TRACE(msg)            wcout << msg
#define TRACE_ACTION(a, k, v) ucout << a << L" (" << k << L", " << v << L")\n"

map<utility::string_t, utility::string_t> dictionary;

void display_json(
    json::value const & jvalue,
    utility::string_t const & prefix)
{
    //wcout << prefix << jvalue.serialize() << endl;
}

const std::function<void(http_request)> handle_get_wrapped(vector<sensor_obs> flows) {
    return ([flows](http_request request) {
        TRACE(L"\nhandle GET\n");

        auto answer = json::value::object();
        answer[U("status")] = json::value::string(U("Okay"));

        display_json(json::value::null(), U("R: "));
        display_json(answer, U("S: "));

        request.reply(status_codes::OK, answer);
    });
}

void handle_get(http_request request)
{
    TRACE(L"\nhandle GET\n");

    auto answer = json::value::object();

    for (auto const & p : dictionary)
    {
        answer[p.first] = json::value::string(p.second);
    }

    display_json(json::value::null(), U("R: "));
    display_json(answer, U("S: "));

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
        catch (http_exception const & e)
        {
            //wcout << e.what() << endl;
        }
    })
        .wait();


    //display_json(answer, L"S: ");

    request.reply(status_codes::OK, answer);
}



const std::function<void(http_request)> handle_post_wrapped(map<utility::string_t, feature_of_interest> feature_map) {
    return ([feature_map](http_request request) {
        TRACE("\nhandle POST\n");

        handle_request(
            request,
            [feature_map](json::value const & jvalue, json::value & answer)
        {
            for (auto const & e : jvalue.as_array())
            {
                if (e.is_string())
                {
                    auto key = e.as_string();
                    auto pos = feature_map.find(key);

                    if (pos == feature_map.end())
                    {
                        answer[key] = json::value::string(U("<nil>"));
                    }
                    else
                    {
                        feature_of_interest feature = pos->second;
                        vector<sensor_obs> flow_history = feature.get_sensor_history();

                        std::vector<web::json::value> flowOut;
                        for (unsigned int i = 0; i < flow_history.size(); i++) {
                            sensor_obs item = flow_history[i];
                            web::json::value vehicle;
                            vehicle[U("flow")] = json::value(item.get_flow());
                            vehicle[U("time")] = json::value::string(item.get_time());
                            flowOut.push_back(vehicle);
                        }
                        answer[U("flows")] = web::json::value::array(flowOut);
                    }
                }
            }
        });
    });
}


void server_session::create_session(map<utility::string_t, feature_of_interest> feature_map, utility::string_t port) {
    string host = "http://0.0.0.0:"; //"http://localhost:"
    utility::string_t address = utility::conversions::to_string_t(host) + port;

    http_listener listener(address);
    wcout << "listening on " << port.c_str() << endl;
    listener.support(methods::GET, handle_get_wrapped(flows));
    listener.support(methods::POST, handle_post_wrapped(feature_map));

    try
    {
        listener
            .open()
            .then([&listener]() {TRACE(L"\nstarting to listen\n"); })
            .wait();

        while (true);
    }
    catch (exception const & e)
    {
        //wcout << e.what() << endl;
    }

}