#ifndef NIWA_DATA_SOURCE_H
#define NIWA_DATA_SOURCE_H

#include "data_source.h"

using namespace std;

class niwa_data_source: public data_source {
public:
    niwa_data_source() {
        _host_url = utility::conversions::to_string_t("https://hydro-sos.niwa.co.nz/");
        initiliased = false;
        data_source_name = "NIWA";
        _observation_types.push_back(observation_type(flow, cumecs, "Discharge"));
    }

    uri_builder get_source_uri(string obs_type = "") {
        uri_builder builder;
        builder.append_query(U("service"), U("SOS"));
        builder.append_query(U("version"), U("2.0.0"));
        builder.append_query(U("request"), utility::conversions::to_string_t(obs_type));
        return builder;
    };

    void process_feature_response(pugi::xml_node responses) {
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
            new_feature = new feature_of_interest(utility::conversions::to_string_t(name), id_t, lat_lon(lat, lon), utility::conversions::to_string_t(data_source_name));
            feature_map[id_t] = new_feature;
        }
    };

    void process_data_availability_response(pugi::xml_node responses) {
        for (pugi::xml_node item : responses.children("gda:dataAvailabilityMember")) {
            pugi::xml_node feature_id = item.child("gda:featureOfInterest");
            string id = feature_id.first_attribute().value();
            utility::string_t id_t = utility::conversions::to_string_t(id);

            pugi::xml_node data_type = item.child("gda:observedProperty");
            string type = data_type.first_attribute().value();

            auto pos = feature_map.find(id_t);
            if (pos == feature_map.end()) {
                wcout << "FEATURE NOT FOUND" << endl;
            }
            else {
                observable obs_type = utils::string_to_observable(type);
                feature_map[id_t]->add_observation_type(observation_type(
                    obs_type,
                    utils::type_to_unit_niwa(obs_type),
                    type
                ));
            }
        }
    }

    void get_all_features() {
        std::vector<unique_ptr<feature_of_interest>> features;
        std::wcout << "Getting features..." << std::endl;
        string res_string = utils::get_xml_response(_host_url, get_source_uri("GetFeatureOfInterest")).get();
        pugi::xml_document doc;
        pugi::xml_parse_result response_all = doc.load_string(res_string.c_str());

        pugi::xml_node responses = doc.child("sos:GetFeatureOfInterestResponse");

        wcout << "got responses" << endl;
        process_feature_response(responses);
        // get data availability for these features
        res_string = utils::get_xml_response(_host_url, get_source_uri("GetDataAvailability")).get();
        pugi::xml_document doc_data_avail;
        pugi::xml_parse_result response_availability = doc_data_avail.load_string(res_string.c_str());

        pugi::xml_node responses_availability = doc_data_avail.child("gda:GetDataAvailabilityResponse");

        process_data_availability_response(responses_availability);
    };

    void process_flow_response(pugi::xml_node doc, std::map<string, sensor_obs> &result, observable type) {
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
        wcout << "Getting flow data, time filter = " << utility::conversions::to_string_t(time_filter).c_str() << endl;
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
