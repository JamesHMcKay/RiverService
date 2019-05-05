#ifndef HILLTOP_DATA_SOURCE_H
#define HILLTOP_DATA_SOURCE_H

#include "data_source.h"

using namespace std;

class hilltop_data_source : public data_source {
public:
    hilltop_data_source(string host_url, string source_name) {
        _host_url = utility::conversions::to_string_t(host_url);
        initiliased = false;
        data_source_name = source_name;
        _observation_types.push_back(observation_type(flow, cumecs, "Flow"));
        _observation_types.push_back(observation_type(stage_height, metres, "Stage"));
        _observation_types.push_back(observation_type(temperature, celcius, "Water Temperature (continuous)"));
    }

    uri_builder get_source_uri(string obs_type) {
        uri_builder builder;
        builder.append_query(U("service"), U("Hilltop"));
        builder.append_query(U("request"), U("SiteList"));
        builder.append_query(U("location"), U("LatLong"));
        builder.append_query(U("Measurement"), utility::conversions::to_string_t(obs_type));
        return builder;
    };

    void process_feature_response(pugi::xml_node responses, observation_type type) {
        for (pugi::xml_node item : responses.children("Site")) {
            string name = item.first_attribute().value();
            if (name != "") {
                pugi::xml_node lat = item.child("Latitude");
                pugi::xml_node lon = item.child("Longitude");

                string lat_str = lat.text().get();
                string lon_str = lon.text().get();
                utility::string_t id_t = utility::conversions::to_string_t(name);

                auto pos = feature_map.find(id_t);
                if (pos == feature_map.end()) {
                    feature_of_interest * new_feature;
                    new_feature = new feature_of_interest(name, id_t, lat_lon(atof(lat_str.c_str()), atof(lon_str.c_str())), data_source_name, { type });
                    feature_map[id_t] = new_feature;
                }
                else {
                    feature_map[id_t]->add_observation_type(type);
                }
            }
        }
    };

    void get_all_features() {
        std::vector<unique_ptr<feature_of_interest>> features;
        std::wcout << "Getting features..." << std::endl;

        for (auto &type : _observation_types) {
            string res_string = utils::get_xml_response(_host_url, get_source_uri(type.get_source_name())).get();
            pugi::xml_document doc;
            pugi::xml_parse_result response_all = doc.load_string(res_string.c_str());

            pugi::xml_node responses = doc.child("HilltopServer");

            process_feature_response(responses, type);
        }
    };


    string get_flow_data(utility::string_t feature_id, string lower_time, string type) {
        string time_filter = "om:phenomenonTime," + lower_time + "/" + utils::get_distant_future_time();
        wcout << "Getting flow data, time filter = " << utility::conversions::to_string_t(time_filter).c_str() << endl;
        http_client client(_host_url);
        uri_builder builder;
        builder.append_query(U("service"), U("SOS"));
        builder.append_query(U("request"), U("GetObservation"));
        builder.append_query(U("FeatureOfInterest"), feature_id);

        // make Flow a parameter
        string_t type_of_obs = utility::conversions::to_string_t(type);
        builder.append_query(U("ObservedProperty"), type_of_obs);
        builder.append_query(U("TemporalFilter"), utility::conversions::to_string_t(time_filter));
        string res_string = utils::get_xml_response(_host_url, builder).get();
        return res_string;
    };

    void process_flow_response(pugi::xml_node doc, std::map<string, sensor_obs> &result, observable type) {
        pugi::xml_node responses = doc.child("wml2:Collection").child("wml2:observationMember").child("om:OM_Observation").child("om:result").child("wml2:MeasurementTimeseries");
        for (pugi::xml_node item : responses.children("wml2:point")) {
            pugi::xml_node feature = item.child("wml2:MeasurementTVP");
            string value = feature.child("wml2:value").text().get();
            string time = feature.child("wml2:time").text().get();
            double value_num = atof(value.c_str());
            auto pos = result.find(time);
            sensor_obs new_flow(value_num, time, type);
            if (pos == result.end()) {
                result[time] = new_flow;
            } else {
                result[time] = pos->second + new_flow;
            }
        }
    };
};

#endif
