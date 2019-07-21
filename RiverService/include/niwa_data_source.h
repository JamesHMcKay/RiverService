#ifndef NIWA_DATA_SOURCE_H
#define NIWA_DATA_SOURCE_H

#include "data_source.h"
#include "hilltop_data_source.h"

using namespace std;

class niwa_data_source: public hilltop_data_source {
public:
    niwa_data_source() {
        _host_url = utility::conversions::to_string_t("https://hydro-sos.niwa.co.nz/");
        initiliased = false;
        data_source_name = "NIWA";
        _source_type_dict = type_dict("Discharge", "Stage", "Water Temperature");
        _source_units = units(1, 1);
    }

    uri_builder get_source_uri(string obs_type = "") {
        uri_builder builder;
        builder.append_query(U("service"), U("SOS"));
        builder.append_query(U("version"), U("2.0.0"));
        builder.append_query(U("request"), utility::conversions::to_string_t(obs_type));
        return builder;
    };

    void process_flow_response(string flow_res_string, std::map<string, sensor_obs> &result, observable type) {
        pugi::xml_document doc;
        pugi::xml_parse_result flow_response_all = doc.load_string(flow_res_string.c_str());


        pugi::xml_node responses = doc.child("sos:GetObservationResponse").child("sos:observationData").child("om:OM_Observation").child("om:result").child("wml2:MeasurementTimeseries");
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

    string get_flow_data(utility::string_t feature_id, string lower_time, string type)
    {
        string time_filter = "om:phenomenonTime," + lower_time + "/" + utils::get_distant_future_time();
        http_client client(_host_url);
        uri_builder builder;
        builder.append_query(U("service"), U("SOS"));
        builder.append_query(U("version"), U("2.0.0"));
        builder.append_query(U("request"), U("GetObservation"));
        builder.append_query(U("FeatureOfInterest"), feature_id);
        string_t type_of_obs = utility::conversions::to_string_t(type);
        builder.append_query(U("ObservedProperty"), type_of_obs);
        builder.append_query(U("TemporalFilter"), utility::conversions::to_string_t(time_filter));
        string res_string = utils::get_xml_response(_host_url, builder).get();
        return res_string;
    }
};

#endif
