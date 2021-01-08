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

    };


    void get_all_features() {
        uri_builder builder;
        string query = "{gauges(limit:999,where: {agency_name:\"" + data_source_name + "\"}){region, agency_name, latitude, longitude, query_id, available_data, river_name, gauge_id, gauge_name}}";
        builder.append_query(U("query"), to_string_t(query));
        string_t strapi_url = utility::conversions::to_string_t("https://api.riverguide.co.nz/graphql");
        string res_string = utils::get_xml_response(strapi_url, builder).get();
        stringstream_t s;
        s << utility::conversions::to_string_t(res_string).c_str();
        web::json::value ret = json::value::parse(s);

        json::value gauges = ret.at(U("data")).at(U("gauges"));
        vector<string_t> result_gauges;

        for (auto const & e : gauges.as_array()) {
            string_t name = e.at(U("gauge_name")).as_string();
            string_t id_t = e.at(U("query_id")).as_string();
            string_t region = e.at(U("region")).as_string();
            string_t river_name = e.at(U("river_name")).as_string();
            double lat = e.at(U("latitude")).as_double();
            double lon = e.at(U("longitude")).as_double();
            string_t data_source_name = e.at(U("agency_name")).as_string();

            //feature_of_interest * new_feature = new feature_of_interest(name, id_t, lat_lon(lat, lon), data_source_name, region, river_name);
            feature_map[id_t] = new feature_of_interest(name, id_t, lat_lon(lat, lon), data_source_name, region, river_name);
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
                    feature_map[id_t]->add_observation_type(observation_type(rainfall, millimetres, _source_type_dict.get_name(rainfall)));
                }
            }
        }

        return;
    }

    string get_flow_data(utility::string_t feature_id, string lower_time, string type) {
        string time_filter = "om:phenomenonTime," + lower_time + "/" + utils::get_distant_future_time();
        //wcout << "Getting data for site = " << feature_id.c_str() << endl;
        http_client client(_host_url);
        uri_builder builder;
        builder.append_query(U("service"), U("SOS"));
        builder.append_query(U("version"), U("2.0.0"));
        builder.append_query(U("request"), U("GetObservation"));
        builder.append_query(U("FeatureOfInterest"), feature_id);

        // make Flow a parameter
        string_t type_of_obs = utility::conversions::to_string_t(type);
        builder.append_query(U("ObservedProperty"), type_of_obs);
        builder.append_query(U("TemporalFilter"), utility::conversions::to_string_t(time_filter));
        string res_string = utils::get_xml_response(_host_url, builder).get();
        return res_string;
    };

    string get_rainfall_data(utility::string_t feature_id, string lower_time, string type) {
        string time_filter = lower_time + "/" + utils::get_distant_future_time();
        //wcout << "Getting rainfall data for site = " << feature_id.c_str() << endl;
        http_client client(_host_url);
        uri_builder builder;
        builder.append_query(U("service"), U("Hilltop"));
        builder.append_query(U("Alignment"), U("00:00"));
        builder.append_query(U("Interval"), U("1 hour"));
        builder.append_query(U("request"), U("GetData"));
        builder.append_query(U("Site"), feature_id);

        string_t type_of_obs = utility::conversions::to_string_t(type);
        builder.append_query(U("Measurement"), type_of_obs);
        builder.append_query(U("TimeInterval"), utility::conversions::to_string_t(time_filter));

        string res_string = utils::get_xml_response(_host_url, builder).get();
        return res_string;
    };

    void process_rainfall_response(string flow_res_string, std::map<string, sensor_obs> &result, observable type) {
        pugi::xml_document doc;
        pugi::xml_parse_result flow_response_all = doc.load_string(flow_res_string.c_str());

        pugi::xml_node responses = doc.child("Hilltop").child("Measurement").child("Data");
        for (pugi::xml_node item : responses.children("E")) {
            string value = item.child("I1").text().get();
            string time = item.child("T").text().get();
            time = time + "+12:00";
            if (value != "") {
                double value_num = atof(value.c_str()) / _source_units.get_units(type);
                auto pos = result.find(time);
                sensor_obs new_flow(value_num, time, type);
                if (pos == result.end()) {
                    result[time] = new_flow;
                }
                else {
                    result[time] = pos->second + new_flow;
                }
            }
        }
    };

    void process_flow_response(string flow_res_string, std::map<string, sensor_obs> &result, observable type) {
        pugi::xml_document doc;
        pugi::xml_parse_result flow_response_all = doc.load_string(flow_res_string.c_str());

        pugi::xml_node responses = doc.child("wml2:Collection").child("wml2:observationMember").child("om:OM_Observation").child("om:result").child("wml2:MeasurementTimeseries");
        for (pugi::xml_node item : responses.children("wml2:point")) {
            pugi::xml_node feature = item.child("wml2:MeasurementTVP");
            string value = feature.child("wml2:value").text().get();
            string time = feature.child("wml2:time").text().get();
            if (value != "") {
                double value_num = atof(value.c_str()) / _source_units.get_units(type);
                auto pos = result.find(time);
                sensor_obs new_flow(value_num, time, type);
                if (pos == result.end()) {
                    result[time] = new_flow;
                }
                else {
                    result[time] = pos->second + new_flow;
                }
            }
        }
    };
};

#endif
