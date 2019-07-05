#ifndef HILLTOP_DATA_SOURCE_H
#define HILLTOP_DATA_SOURCE_H

#include "data_source.h"

using namespace std;
using namespace utility::conversions;

class hilltop_data_source : public data_source {
public:
    hilltop_data_source(string host_url, string source_name, units source_units = units(), type_dict source_type_dict = type_dict()) {
        _host_url = utility::conversions::to_string_t(host_url);
        initiliased = false;
        data_source_name = source_name;
        _source_type_dict = source_type_dict;
        _source_units = source_units;

        // the following deprecated
        _observation_types.push_back(observation_type(flow, cumecs, "Flow"));
        _observation_types.push_back(observation_type(stage_height, metres, "Stage"));
        _observation_types.push_back(observation_type(temperature, celcius, "Water Temperature (continuous)"));
    }

    hilltop_data_source() {}

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
                    //new_feature = new feature_of_interest(name, id_t, lat_lon(atof(lat_str.c_str()), atof(lon_str.c_str())), data_source_name, { type });
                    feature_map[id_t] = new_feature;
                }
                else {
                    feature_map[id_t]->add_observation_type(type);
                }
            }
        }
    };


    void get_all_features() {
        uri_builder builder;
        string query = "{gauges(where: {agency_name:\"" + data_source_name + "\"}){region, agency_name, latitude, longitude, query_id, available_data, river_name, gauge_id, gauge_name}}";
        builder.append_query(U("query"), to_string_t(query));
        string_t strapi_url = utility::conversions::to_string_t("https://rapidsapi.herokuapp.com/graphql");
        string res_string = utils::get_xml_response(strapi_url, builder).get();
        stringstream_t s;
        s << utility::conversions::to_string_t(res_string).c_str();
        web::json::value ret = json::value::parse(s);

        json::value gauges = ret.at(U("data")).at(U("gauges"));
        vector<string_t> result_gauges;

        for (auto const & e : gauges.as_array()) {
            string_t name = e.at(U("gauge_name")).as_string();
            string_t id_t = e.at(U("gauge_id")).as_string();
            string_t region = e.at(U("region")).as_string();
            string_t river_name = e.at(U("river_name")).as_string();
            double lat = e.at(U("latitude")).as_double();
            double lon = e.at(U("longitude")).as_double();
            string_t data_source_name = e.at(U("agency_name")).as_string();

            feature_of_interest * new_feature;
            new_feature = new feature_of_interest(name, id_t, lat_lon(lat, lon), data_source_name, region, river_name);
            feature_map[id_t] = new_feature;
            for (auto const & data_type : e.at(U("available_data")).as_array()) {
                string_t data_type_str = data_type.as_string();

                if (data_type_str == to_string_t("flow")) {
                    feature_map[id_t]->add_observation_type(observation_type(flow, cumecs, _source_type_dict.get_name(flow)));
                }
                if (data_type_str == to_string_t("temp")) {
                    feature_map[id_t]->add_observation_type(observation_type(temperature, celcius, _source_type_dict.get_name(temperature)));
                }
                if (data_type_str == to_string_t("stage_height")) {
                    feature_map[id_t]->add_observation_type(observation_type(stage_height, metres, _source_type_dict.get_name(stage_height)));
                }
                if (data_type_str == to_string_t("rainfall")) {
                    feature_map[id_t]->add_observation_type(observation_type(rainfall, metres, _source_type_dict.get_name(rainfall)));
                }
            }
        }

        return;
    }

/*
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
    };*/


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

    void process_flow_response(string flow_res_string, std::map<string, sensor_obs> &result, observable type) {
        pugi::xml_document doc;
        pugi::xml_parse_result flow_response_all = doc.load_string(flow_res_string.c_str());

        pugi::xml_node responses = doc.child("wml2:Collection").child("wml2:observationMember").child("om:OM_Observation").child("om:result").child("wml2:MeasurementTimeseries");
        for (pugi::xml_node item : responses.children("wml2:point")) {
            pugi::xml_node feature = item.child("wml2:MeasurementTVP");
            string value = feature.child("wml2:value").text().get();
            string time = feature.child("wml2:time").text().get();
            double value_num = atof(value.c_str()) / _source_units.get_units(type);
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
