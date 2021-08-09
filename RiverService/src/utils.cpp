#include "utils.h"

using namespace std;
using namespace Concurrency;
using namespace chrono;

namespace utils {
    string ref_time_str() {
        return "2000-12-01T01:00:00.000Z";
    }

    time_t my_timegm(tm* _Tm)
    {
        auto t = mktime(_Tm);
        return t + (mktime(localtime(&t)) - mktime(gmtime(&t)));
    }

    chrono::system_clock::time_point convert_time_str(string time) {
        std::istringstream input(time);
        tm t{};
        input >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
        return chrono::system_clock::from_time_t(my_timegm(&t));
    }

    // fix this to get the actual date time
    string get_distant_future_time() {
        return "2100-06-01T00:00:00.000Z";
    }

    string get_time_utc(chrono::system_clock::time_point time_now, string time_zone_diff) {
        int offset = std::stoi(time_zone_diff);
        time_now += -chrono::hours(offset) + chrono::seconds(1);
        auto time_t_now = std::chrono::system_clock::to_time_t(time_now);
        std::ostringstream ss;
        ss << std::put_time(gmtime(&time_t_now), "%FT%TZ");
        return ss.str();
    }

    system_clock::time_point get_time_point_utc(string time_str) {
        int time_str_length = time_str.size();
        string suffix = time_str.substr(time_str_length - 1, time_str_length - 1);
        int offset = 0;
        if (suffix != "Z") {
            string time_zone = time_str.substr(time_str_length - 5, time_str_length - 1);
            offset = std::stoi(time_zone);
        }
        system_clock::time_point first_time = utils::convert_time_str(time_str);
        first_time += -chrono::hours(offset);
        return first_time;
    }

    pplx::task<string> parse_xml_response(http_client client, string path_query_fragment) {
      return client.request(methods::GET, path_query_fragment).then([](task<http_response> response_task) {
            http_response response;
            try {
                response = response_task.get();
            }
            catch (const std::exception& e) {
                wcout << "request error: " << e.what() << endl;
                wcout << "Response status code: " << response.status_code() << std::endl;
                response.set_status_code(status_codes::RequestTimeout);
            }
            string result = "";
            if (response.status_code() == status_codes::OK) {
                std::wostringstream stream;
                std::wcout << stream.str();
                stream.str(std::wstring());
                std::wcout << stream.str();
                auto bodyStream = response.body();
                streams::stringstreambuf sbuffer;
                auto& target = sbuffer.collection();
                bodyStream.read_to_end(sbuffer).get();
                stream.str(std::wstring());
                std::wcout << stream.str();
                result = target.c_str();
            }
            return result;
        });
    }

    pplx::task<string> get_xml_response(string_t host_url, uri_builder uri) {
        web::http::client::http_client_config client_config;
        client_config.set_timeout(std::chrono::seconds(10));
        http_client client(host_url, client_config);
        auto path_query_fragment = uri.to_string();
        // wcout << "path_query_fragment = " << path_query_fragment.c_str() << endl;
        return parse_xml_response(client, path_query_fragment);
    }

    string observable_to_string(observable type) {
        string result;
        switch (type)
        {
            case flow: result = "flow"; break;
            case stage_height: result = "stage_height"; break;
            case temperature: result = "temperature"; break;
            case rainfall: result = "rainfall"; break;
            default: result = "value";
        }
        return result;
    }

    observable string_to_observable(string type) {
        if (type == "Discharge") {
            return flow;
        }
        if (type == "pH") {
            return pH;
        }
        if (type == "Height of Gauge (River Stage)") {
            return stage_height;
        }
        return flow;
    }

    unit type_to_unit_niwa(observable type) {
        unit result;
        switch (type)
        {
        case flow: result = cumecs; break;
        case stage_height: result = metres; break;
        case pH: result = none;
        default: result = none;
        }
        return result;
    }
}
