#ifndef NIWA_DATA_SOURCE_H
#define NIWA_DATA_SOURCE_H

#include "data_source.h"

using namespace std;

class niwa_data_source: public data_source {
public:
    niwa_data_source() {
        //_host_url = utility::conversions::to_string_t("https://hydro-sos.niwa.co.nz/");
        _host_url = utility::conversions::to_string_t("http://localhost:8080");
        initiliased = false;
        data_source_name = "NIWA";
    }

    uri_builder get_source_uri() {
        uri_builder builder;
        builder.append_query(U("service"), U("SOS"));
        builder.append_query(U("version"), U("2.0.0"));
        builder.append_query(U("request"), U("GetFeatureOfInterest"));
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
            new_feature = new feature_of_interest(name, id_t, lat_lon(lat, lon), data_source_name);
            feature_map[id_t] = new_feature;
        }
    };

    void get_all_features() {
        std::vector<unique_ptr<feature_of_interest>> features;
        std::wcout << L"Getting features..." << std::endl;
        string res_string = utils::get_xml_response(_host_url, get_source_uri()).get();
        pugi::xml_document doc;
        pugi::xml_parse_result response_all = doc.load_string(res_string.c_str());
        std::wcout << "got response = " << res_string.c_str() << std::endl;

        pugi::xml_node responses = doc.child("sos:GetFeatureOfInterestResponse");

        wcout << "got responses" << endl;
        process_feature_response(responses);
    };

    void process_flow_response(pugi::xml_node doc, std::vector<sensor_obs> &result) {
        pugi::xml_node responses = doc.child("sos:GetObservationResponse").child("sos:observationData").child("om:OM_Observation").child("om:result").child("wml2:MeasurementTimeseries");
        for (pugi::xml_node item : responses.children("wml2:point")) {
            pugi::xml_node feature = item.child("wml2:MeasurementTVP");
            string value = feature.child("wml2:value").text().get();
            string time = feature.child("wml2:time").text().get();

            double value_num = atof(value.c_str());
            sensor_obs new_flow(value_num, time, "units");
            result.push_back(new_flow);
        }
    };


    string get_flow_data(utility::string_t feature_id, string lower_time)
    {
        string time_filter = "om:phenomenonTime," + lower_time + "/" + utils::get_distant_future_time();
        wcout << "Getting flow data, time filter = " << utility::conversions::to_string_t(time_filter).c_str() << endl;
        http_client client(_host_url);
        uri_builder builder;
        builder.append_query(U("service"), U("SOS"));
        builder.append_query(U("version"), U("2.0.0"));
        builder.append_query(U("request"), U("GetObservation"));
        builder.append_query(U("FeatureOfInterest"), feature_id);
        builder.append_query(U("ObservedProperty"), U("Discharge"));
        builder.append_query(U("TemporalFilter"), utility::conversions::to_string_t(time_filter));
        string res_string = utils::get_xml_response(_host_url, builder).get();
        return res_string;
    }
};

#endif
