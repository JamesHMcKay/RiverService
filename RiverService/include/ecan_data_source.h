#ifndef ECAN_DATA_SOURCE_H
#define ECAN_DATA_SOURCE_H

#include "data_source.h"

using namespace std;
using namespace utility::conversions;

class ecan_data_source : public data_source {
public:
    ecan_data_source() {
        _host_url = utility::conversions::to_string_t("http://data.ecan.govt.nz/data/79/Water/River%20stage%20flow%20data%20for%20individual%20site/JSON");
        initiliased = false;
        data_source_name = "Environment Canterbury";
        _source_units = units(1, 1);
        _source_type_dict = type_dict("River Flow", "River Stage", "Temperature");
    }

    uri_builder get_source_uri(string obs_type = "") {
        uri_builder builder;
        builder.append_query(U("service"), U("SOS"));
        builder.append_query(U("version"), U("2.0.0"));
        builder.append_query(U("request"), utility::conversions::to_string_t(obs_type));
        return builder;
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
                if (data_type_str == to_string_t("stage")) {
                    feature_map[id_t]->add_observation_type(observation_type(stage_height, metres, _source_type_dict.get_name(stage_height)));
                }
            }
        }

        return;
    }

    void process_flow_response(string flow_res_string, std::map<string, sensor_obs> &result, observable type) {
        stringstream_t s;
        s << utility::conversions::to_string_t(flow_res_string).c_str();
        web::json::value ret = json::value::parse(s);
        if (ret.at(U("data")).is_null()) {
            return;
        }

        json::value data_points = ret.at(U("data")).at(U("item"));
        vector<string_t> result_gauges;

        for (auto const & e : data_points.as_array()) {
            string_t value = e.at(U("Value")).as_string();
            string time = utility::conversions::to_utf8string(e.at(U("DateTime")).as_string());
            double value_num = atof(utility::conversions::to_utf8string(value).c_str());
            auto pos = result.find(time);
            sensor_obs new_flow(value_num, time, type);
            if (pos == result.end()) {
                result[time] = new_flow;
            }
            else {
                result[time] = pos->second + new_flow;
            }
        }
    };


    string get_flow_data(utility::string_t feature_id, string lower_time, string type)
    {
        uri_builder builder;
        builder.append_query(U("SiteNo"), feature_id);
        builder.append_query(U("Period"), U("2_Weeks"));
        builder.append_query(U("StageFlow"), utility::conversions::to_string_t(type));

        string res_string = utils::get_xml_response(_host_url, builder).get();

        return res_string;
    }
};

#endif
