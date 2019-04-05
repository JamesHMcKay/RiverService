#include "feature.h"

using namespace chrono;

bool accept_observation(sensor_obs first, sensor_obs second, duration<double> tolerance) {
    system_clock::time_point first_time = utils::convert_time_str(first.get_time_str());
    system_clock::time_point second_time = utils::convert_time_str(second.get_time_str());

    if (second_time - first_time < tolerance) {
        return false;
    }
    return true;
}

void feature_of_interest::filter_observations(vector<sensor_obs> obs) {
    int num_of_obs = obs.size();
    duration<double > tolerance = hours(1);

    obs_store.add_to_top(obs[0]);
    sensor_obs last_accepted_point = obs[0];

    for (int i = 1; i < num_of_obs; i++) {
        if (accept_observation(last_accepted_point, obs[i], tolerance)) {
            obs_store.add_to_top(obs[i]);
            last_accepted_point = obs[i];
        }
    }

    if (num_of_obs > 2) {
        set_update_time(obs[num_of_obs - 1], obs[num_of_obs - 2]);
    }

    obs_store.remove_old_points(chrono::hours(3));
}


void feature_of_interest::update() {
    // get an example flow response
    std::vector<sensor_obs> flows;

    string lower_time;
    int num_points = sensor_history.size();
    if (num_points == 0) {
        lower_time = utils::ref_time_str();
    } else {
        string latest_time = obs_store.get_first()->value.get_time_str();
        int time_str_length = latest_time.size();
        string time_zone = latest_time.substr(time_str_length - 5, time_str_length - 1);
        lower_time = utils::get_time_utc(utils::convert_time_str(latest_time), time_zone);
    }

    string flow_res_string = get_flow_data(get_id(), lower_time).get();
    pugi::xml_document doc;
    pugi::xml_parse_result flow_response_all = doc.load_string(flow_res_string.c_str());

    pugi::xml_node flow_responses = doc.child("sos:GetObservationResponse").child("sos:observationData").child("om:OM_Observation").child("om:result").child("wml2:MeasurementTimeseries");

    wcout << "got flow responses" << endl;
    process_flow_response(flow_responses, flows);
    filter_observations(flows);
}

pplx::task<string> feature_of_interest::get_flow_data(utility::string_t feature_id, string lower_time)
{
    string time_filter = "om:phenomenonTime," + lower_time + "/" + utils::get_distant_future_time();
    wcout << "Getting flow data, time filter = " << utility::conversions::to_string_t(time_filter).c_str() << endl;
    string_t url = utility::conversions::to_string_t(_host_url);
    http_client client(url);
    uri_builder builder;
    builder.append_query(U("service"), U("SOS"));
    builder.append_query(U("version"), U("2.0.0"));
    builder.append_query(U("request"), U("GetObservation"));
    builder.append_query(U("FeatureOfInterest"), feature_id);
    builder.append_query(U("ObservedProperty"), U("Discharge"));
    builder.append_query(U("TemporalFilter"), utility::conversions::to_string_t(time_filter));
    auto path_query_fragment = builder.to_string();

    return client.request(methods::GET, path_query_fragment).then([](http_response response)
    {
        std::wostringstream stream;
        //stream << L"Server returned returned status code " << response.status_code() << L'.' << std::endl;
        std::wcout << stream.str();

        stream.str(std::wstring());
        //stream << L"Content type: " << response.headers().content_type() << std::endl;
        //stream << L"Content length: " << response.headers().content_length() << L"bytes" << std::endl;
        std::wcout << stream.str();

        auto bodyStream = response.body();
        Concurrency::streams::stringstreambuf sbuffer;
        auto& target = sbuffer.collection();

        bodyStream.read_to_end(sbuffer).get();

        stream.str(std::wstring());
        std::wcout << stream.str();

        string result = target.c_str();
        return result;
    });
}

void feature_of_interest::process_flow_response(pugi::xml_node responses, std::vector<sensor_obs> &result) {
    for (pugi::xml_node item : responses.children("wml2:point"))
    {
        pugi::xml_node feature = item.child("wml2:MeasurementTVP");
        string value = feature.child("wml2:value").text().get();
        string time = feature.child("wml2:time").text().get();

        double value_num = atof(value.c_str());
        sensor_obs new_flow(value_num, time, "units");
        result.push_back(new_flow);
    }
}