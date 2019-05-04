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

    for (auto &entry : feature_map) {
        count++;
        if (count < 10) {
        update_feature(entry.second);
        update_queue.push(entry.second);
        }
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

void data_source::update_sources() {
    last_updated = system_clock::now();

    system_clock::time_point current_time = system_clock::now();
    // get flows
    wcout << "updating flows" << endl;
    chrono::duration<double> current_time_ref = current_time - utils::convert_time_str(utils::ref_time_str());
    wcout << "The current time reference is " << current_time_ref.count() << endl;

    vector<feature_of_interest*> updated_features;
    while (!update_queue.empty() && update_queue.top()->next_update_time < current_time_ref) {
        feature_of_interest* temp_feature = update_queue.top();
        wcout << "updating feature with name " << temp_feature->get_name().c_str() << endl;

        update_feature(temp_feature);

        update_queue.pop();
        updated_features.push_back(temp_feature);
    }

    for (auto &feature_item : updated_features) {
        update_queue.push(feature_item);
    }

    wcout << "queue is updated, the new order is:" << endl;
    std::priority_queue<feature_of_interest*, std::vector<feature_of_interest*>, OrderUpdateQueue> update_queue_copy = update_queue;
    while (!update_queue_copy.empty()) {
        feature_of_interest* temp_feature = update_queue_copy.top();
        chrono::duration <double> time = temp_feature->next_update_time;
        wcout << temp_feature->get_name().c_str() << ", with next update time = " << time.count() << endl;
        update_queue_copy.pop();
    }
}

void data_source::update_feature(feature_of_interest* feature_to_update) {
    // this needs to be generalised to handle features of different, or multiple types
    // perhaps just iterate through for each type since multiple requests are required
    // give each feature a list of types
    feature_to_update->set_last_checked_for_update_time();
    std::vector<sensor_obs> flows;

    string lower_time = feature_to_update->get_lower_time();

    string flow_res_string = get_flow_data(feature_to_update->get_id(), lower_time);
    pugi::xml_document doc;
    pugi::xml_parse_result flow_response_all = doc.load_string(flow_res_string.c_str());
    wcout << "got flow responses" << endl;
    // add a type or units parameter
    process_flow_response(doc, flows);
    // generalise this function to type a "type of obs" parameter
    feature_to_update->filter_observations(flows);
}