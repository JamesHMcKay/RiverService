#include "data_source.h"

using namespace std;
using namespace Concurrency;

map<utility::string_t, feature_of_interest*> data_source::get_available_features() {
    if (!initiliased) {
        initiliased = true;
        get_all_features();
    }

    // create the initial priority_queue
    int count = 0;

    wcout << "creating feature queue for source:" << data_source_name.c_str() << endl;

    for (auto &entry : feature_map) {
        count++;
        //if (count < 10) {
        //if (entry.second->get_name() == utility::conversions::to_string_t("Are Are Creek at Kaituna Tuamarina Track"))
        //{
        update_feature(entry.second);
        update_queue.push(entry.second);
        //}
        //}
    }

    wcout << "queue is created, the current order is:" << endl;
    std::priority_queue<feature_of_interest*, std::vector<feature_of_interest*>, OrderUpdateQueue> update_queue_copy = update_queue;
    while (!update_queue_copy.empty()) {
        feature_of_interest* temp_feature = update_queue_copy.top();
        chrono::duration <double> time = temp_feature->next_update_time;
        wcout << temp_feature->get_name().c_str() << ", with next update time = " << time.count() << endl;
        update_queue_copy.pop();
    }
    return feature_map;
}

void data_source::process_rainfall_response(string flow_res_string, std::map<string, sensor_obs>& result, observable type)
{
}

string data_source::get_rainfall_data(utility::string_t feature_id, string lower_time, string type)
{
    return string();
}

void data_source::update_sources() {
    last_updated = system_clock::now();

    system_clock::time_point current_time = system_clock::now();
    // get flows
    wcout << "updating data for:" << data_source_name.c_str() << endl;
    chrono::duration<double> current_time_ref = current_time - utils::convert_time_str(utils::ref_time_str());
    wcout << "The current time reference is " << current_time_ref.count() << endl;

    vector<feature_of_interest*> updated_features;
    while (!update_queue.empty() && update_queue.top()->next_update_time < current_time_ref) {
        feature_of_interest* temp_feature = update_queue.top();
        //wcout << "updating feature with name " << temp_feature->get_name().c_str() << endl;
        try {
            update_feature(temp_feature);
        } catch (const std::exception& e) {
            std::cout << "exception was caught '" << e.what() << "'\n";
        }

        update_queue.pop();
        updated_features.push_back(temp_feature);
    }

    for (auto &feature_item : updated_features) {
        update_queue.push(feature_item);
    }

    //wcout << "queue is updated, the new order is:" << endl;
    std::priority_queue<feature_of_interest*, std::vector<feature_of_interest*>, OrderUpdateQueue> update_queue_copy = update_queue;
    while (!update_queue_copy.empty()) {
        feature_of_interest* temp_feature = update_queue_copy.top();
        chrono::duration <double> time = temp_feature->next_update_time;
        //wcout << temp_feature->get_name().c_str() << ", with next update time = " << time.count() << endl;
        update_queue_copy.pop();
    }
}

void data_source::update_feature(feature_of_interest* feature_to_update) {
    // this needs to be generalised to handle features of different, or multiple types
    // perhaps just iterate through for each type since multiple requests are required
    // give each feature a list of types

    feature_to_update->set_last_checked_to_now();
    string lower_time = feature_to_update->get_lower_time();
    std::map<string, sensor_obs> values;
    for (auto &type : feature_to_update->get_observation_types()) {

        if (type.get_source_name() == "Rainfall") {
            string flow_res_string = get_rainfall_data(feature_to_update->get_id(), lower_time, type.get_source_name());
            if (flow_res_string != "") {
                process_rainfall_response(flow_res_string, values, type.get_obs_type());
            }
        }
        else {
            string flow_res_string = get_flow_data(feature_to_update->get_id(), lower_time, type.get_source_name());
            if (flow_res_string != "") {
                process_flow_response(flow_res_string, values, type.get_obs_type());
            }
        }
    }
    vector<sensor_obs> result;
    for (const auto &s : values) {
        result.push_back(s.second);
    }
    feature_to_update->add_obsevations(result);
}

std::pair<int, int> data_source::create_data_source_summary() {
    int num_passed = 0;
    int num_features = 0;
    for (auto &feature_item : feature_map) {
        num_passed += feature_item.second->status;
        num_features += 1;
    }
    return std::make_pair(num_features, num_passed);
}