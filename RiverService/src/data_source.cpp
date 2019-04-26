#include "data_source.h"

using namespace std;
using namespace Concurrency;

pplx::task<string> data_source::get_features_task() {
    web::http::client::http_client_config client_config;
    client_config.set_timeout(std::chrono::seconds(5));

    http_client client(_host_url, client_config);

    auto path_query_fragment = get_source_uri().to_string();

    return client.request(methods::GET, path_query_fragment).then([](http_response response)
    {
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

void data_source::get_available_features(map<utility::string_t, feature_of_interest*> &features_map) {
    if (!initiliased) {
        initiliased = true;
        get_all_features(features_map);
    }
}
