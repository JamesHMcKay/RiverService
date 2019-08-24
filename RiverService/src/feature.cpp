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
    duration<double > tolerance = minutes(0);
    if (obs_store.length == 0 && num_of_obs > 0) {
        obs_store.add_to_top(obs[0]);
        status = 1;
    }

    for (int i = 1; i < num_of_obs; i++) {
        if (accept_observation(obs_store.get_first()->value, obs[i], tolerance)) {
            obs_store.add_to_top(obs[i]);
        }
    }
    //if (num_of_obs > 2) {
    //    set_update_time(obs[num_of_obs - 1], obs[num_of_obs - 2]);
    //} else {
    //    system_clock::time_point current_time = system_clock::now();
    //    chrono::duration<double> current_time_ref = current_time - utils::convert_time_str(utils::ref_time_str());
    //    next_update_time = current_time_ref + tolerance;
    //}

    system_clock::time_point current_time = system_clock::now();
    chrono::duration<double> current_time_ref = current_time - utils::convert_time_str(utils::ref_time_str());
    next_update_time = current_time_ref + tolerance;

    obs_store.remove_old_points(chrono::hours(720));
}
