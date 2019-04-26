#ifndef NIWA_DATA_SOURCE_H
#define NIWA_DATA_SOURCE_H

#include "data_source.h"

using namespace std;

class niwa_data_source: public data_source {
public:
    niwa_data_source() {
        _host_url = utility::conversions::to_string_t("https://hydro-sos.niwa.co.nz/");
        initiliased = false;
    }

    uri_builder get_source_uri() {
        uri_builder builder;
        builder.append_query(U("service"), U("SOS"));
        builder.append_query(U("version"), U("2.0.0"));
        builder.append_query(U("request"), U("GetFeatureOfInterest"));
        return builder;
    };

    void process_feature_response(pugi::xml_node responses, map<utility::string_t, feature_of_interest*> &features_map) {
        for (pugi::xml_node item : responses.children("sos:featureMember")) {
            pugi::xml_node feature = item.child("sams:SF_SpatialSamplingFeature");
            string name = feature.child("gml:name").text().get();
            string id = feature.child("gml:identifier").text().get();
            string position = feature.child("sams:shape").child("ns:Point").child("ns:pos").text().get();

            utility::string_t id_t = utility::conversions::to_string_t(id);

            std::vector<std::string> position_element;
            boost::split(position_element, position, [](char c) {return c == ' '; });
            double lat = atof(position_element[0].c_str());
            double lon = atof(position_element[1].c_str());

            feature_of_interest * new_feature;
            new_feature = new niwa_feature(name, id_t, lat_lon(lat, lon), _host_url);
            features_map[id_t] = new_feature;
        }
    };

    void get_all_features(map<utility::string_t, feature_of_interest*> &features_map) {
        std::vector<unique_ptr<feature_of_interest>> features;
        std::wcout << L"Getting features..." << std::endl;
        string res_string = get_features_task().get();
        pugi::xml_document doc;
        pugi::xml_parse_result response_all = doc.load_string(res_string.c_str());

        pugi::xml_node responses = doc.child("sos:GetFeatureOfInterestResponse");

        wcout << "got responses" << endl;
        process_feature_response(responses, features_map);
        /*for (unsigned int i = 0; i < features.size(); i++) {
            feature_of_interest* next_feature = features[i];
            features_map[next_feature->get_id()] = next_feature;
            wcout << "got feature id = " << next_feature->get_id().c_str() << endl;
        }*/
    };
};

#endif
