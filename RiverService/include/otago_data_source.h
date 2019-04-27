#ifndef OTAGO_DATA_SOURCE_H
#define OTAGO_DATA_SOURCE_H

#include "data_source.h"

using namespace std;

class otago_data_source : public data_source {
public:
    otago_data_source() {
        _host_url = utility::conversions::to_string_t("http://gisdata.orc.govt.nz/Hilltop/Global.hts");
        initiliased = false;
    }

    uri_builder get_source_uri() {
        uri_builder builder;
        builder.append_query(U("service"), U("Hilltop"));
        builder.append_query(U("request"), U("SiteList"));
        builder.append_query(U("location"), U("LatLong"));
        builder.append_query(U("Measurement"), U("Flow"));
        return builder;
    };

    void process_feature_response(pugi::xml_node responses, map<utility::string_t, feature_of_interest*> &features_map) {
        for (pugi::xml_node item : responses.children("Site")) {
            string name = item.first_attribute().value();
            if (name != "") {
                pugi::xml_node lat = item.child("Latitude");
                pugi::xml_node lon = item.child("Longitude");

                string lat_str = lat.text().get();
                string lon_str = lon.text().get();
                utility::string_t id_t = utility::conversions::to_string_t(name);

                feature_of_interest * new_feature;
                new_feature = new otago_feature(name, id_t, lat_lon(atof(lat_str.c_str()), atof(lon_str.c_str())), _host_url);
                features_map[id_t] = new_feature;
            }
        }
    };

    void get_all_features(map<utility::string_t, feature_of_interest*> &features_map) {
        std::vector<unique_ptr<feature_of_interest>> features;
        std::wcout << L"Getting features..." << std::endl;
        string res_string = get_features_task().get();
        pugi::xml_document doc;
        pugi::xml_parse_result response_all = doc.load_string(res_string.c_str());

        pugi::xml_node responses = doc.child("HilltopServer");

        process_feature_response(responses, features_map);
    };
};

#endif
