#include <xlocale>
#include "server.h"
#include "pugixml.hpp"
#include "feature.h"
#include "utils.h"

#include <iostream>
#include <string>
#include <sstream>
#include <map>

using namespace std;

using namespace web::http;
using namespace web::http::client;
using namespace web::json;

using namespace utils;

map<utility::string_t, feature_of_interest> get_features() {
    std::vector<feature_of_interest> features;
    map <utility::string_t, feature_of_interest> features_map;

    std::wcout << L"Calling HTTPGetAsync..." << std::endl;
    string res_string = HTTPGetAsync().get();
    pugi::xml_document doc;
    pugi::xml_parse_result response_all = doc.load_string(res_string.c_str());

    pugi::xml_node responses = doc.child("sos:GetFeatureOfInterestResponse");

    wcout << "got responses" << endl;
    process_feature_response(responses, features);
    for (unsigned int i = 0; i < features.size(); i++) {
        feature_of_interest next_feature = features[i];
        features_map[next_feature.get_id()] = next_feature;
        wcout << "got feature id = " << next_feature.get_id().c_str() << endl;
    }
    return features_map;
}

void get_flows(feature_of_interest &feature) {
    // get an example flow response
    std::vector<sensor_obs> flows;

    string flow_res_string = get_flow_data(feature.get_id()).get();
    pugi::xml_document doc;
    pugi::xml_parse_result flow_response_all = doc.load_string(flow_res_string.c_str());

    pugi::xml_node flow_responses = doc.child("sos:GetObservationResponse").child("sos:observationData").child("om:OM_Observation").child("om:result").child("wml2:MeasurementTimeseries");

    wcout << "got responses" << endl;
    process_flow_response(flow_responses, flows);

    //for (unsigned int i = 0; i < flows.size(); i++) {
        //cout << "response number " << i << endl;
    //    flows[i].print_content();
    //}
    feature.add_sensor_obs(flows);
}

int main(int argc, char *argv[]) {
    utility::string_t port = U("5000");
    if (argc == 2) {
        port = utility::conversions::to_string_t(argv[1]);
    }

    server_session server;
    map<utility::string_t, feature_of_interest> features_map = get_features();
    // for (auto const& x : features_map) {
    //     get_flows(features_map[x.first]);
    // }
    get_flows(features_map[utility::conversions::to_string_t("3722")]);
    server.create_session(features_map, port);
    return 0;
}