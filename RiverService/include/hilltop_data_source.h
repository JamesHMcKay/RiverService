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
    }

    uri_builder get_source_uri() {
        uri_builder builder;
        builder.append_query(U("service"), U("Hilltop"));
        builder.append_query(U("request"), U("SiteList"));
        builder.append_query(U("location"), U("LatLong"));
        builder.append_query(U("Measurement"), U("Flow"));
        return builder;
    };

    void process_feature_response(pugi::xml_node responses) {
        for (pugi::xml_node item : responses.children("Site")) {
            string name = item.first_attribute().value();
            if (name != "") {
                pugi::xml_node lat = item.child("Latitude");
                pugi::xml_node lon = item.child("Longitude");

                string lat_str = lat.text().get();
                string lon_str = lon.text().get();
                utility::string_t id_t = utility::conversions::to_string_t(name);

                feature_of_interest * new_feature;
                new_feature = new feature_of_interest(name, id_t, lat_lon(atof(lat_str.c_str()), atof(lon_str.c_str())), data_source_name);
                feature_map[id_t] = new_feature;
            }
        }
    };

    void get_all_features() {
        std::vector<unique_ptr<feature_of_interest>> features;
        std::wcout << L"Getting features..." << std::endl;
        string res_string = utils::get_xml_response(_host_url, get_source_uri()).get();
        pugi::xml_document doc;
        pugi::xml_parse_result response_all = doc.load_string(res_string.c_str());

        pugi::xml_node responses = doc.child("HilltopServer");

        process_feature_response(responses);
    };


    string get_flow_data(utility::string_t feature_id, string lower_time) {
        string time_filter = "om:phenomenonTime," + lower_time + "/" + utils::get_distant_future_time();
        wcout << "Getting flow data, time filter = " << utility::conversions::to_string_t(time_filter).c_str() << endl;
        http_client client(_host_url);
        uri_builder builder;
        builder.append_query(U("service"), U("SOS"));
        builder.append_query(U("request"), U("GetObservation"));
        builder.append_query(U("FeatureOfInterest"), feature_id);

        // make Flow a parameter
        builder.append_query(U("ObservedProperty"), U("Flow"));
        builder.append_query(U("TemporalFilter"), utility::conversions::to_string_t(time_filter));
        string res_string = utils::get_xml_response(_host_url, builder).get();
        return res_string;
    };

    void process_flow_response(pugi::xml_node doc, std::vector<sensor_obs> &result) {
        pugi::xml_node responses = doc.child("wml2:Collection").child("wml2:observationMember").child("om:OM_Observation").child("om:result").child("wml2:MeasurementTimeseries");
        for (pugi::xml_node item : responses.children("wml2:point")) {
            pugi::xml_node feature = item.child("wml2:MeasurementTVP");
            string value = feature.child("wml2:value").text().get();
            string time = feature.child("wml2:time").text().get();

            double value_num = atof(value.c_str());
            sensor_obs new_flow(value_num, time, "units");
            result.push_back(new_flow);
        }
    };
};

#endif
