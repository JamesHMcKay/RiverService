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

    if (obs_store.length == 0 && num_of_obs > 0) {
        obs_store.add_to_top(obs[0]);
    }

    for (int i = 1; i < num_of_obs; i++) {
        if (accept_observation(obs_store.get_first()->value, obs[i], tolerance)) {
            obs_store.add_to_top(obs[i]);
        }
    }

    if (num_of_obs > 2) {
        set_update_time(obs[num_of_obs - 1], obs[num_of_obs - 2]);
    }

    obs_store.remove_old_points(chrono::hours(3));
}


void feature_of_interest::update() {
    last_checked_for_updates = chrono::system_clock::now();
    std::vector<sensor_obs> flows;

    string lower_time;
    if (obs_store.length == 0) {
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
    wcout << "got flow responses" << endl;
    process_flow_response(doc, flows);
    filter_observations(flows);
}
