#ifndef OTAGO_FEATURE_H
#define OTAGO_FEATURE_H

#include "feature.h"

class otago_feature : public feature_of_interest {
public:
    otago_feature(string name, utility::string_t id, lat_lon position, string_t host_url) {
        _name = name;
        _position = position;
        _id = id;
        _host_url = host_url;
    }

    pplx::task<string> get_flow_data(utility::string_t feature_id, string lower_time)
    {
        string time_filter = "om:phenomenonTime," + lower_time + "/" + utils::get_distant_future_time();
        wcout << "Getting flow data, time filter = " << utility::conversions::to_string_t(time_filter).c_str() << endl;
        http_client client(_host_url);
        uri_builder builder;
        builder.append_query(U("service"), U("SOS"));
        builder.append_query(U("request"), U("GetObservation"));
        builder.append_query(U("FeatureOfInterest"), feature_id);
        builder.append_query(U("ObservedProperty"), U("Flow"));
        builder.append_query(U("TemporalFilter"), utility::conversions::to_string_t(time_filter));
        auto path_query_fragment = builder.to_string();

        return client.request(methods::GET, path_query_fragment).then([](http_response response)
        {
            std::wostringstream stream;
            std::wcout << stream.str();

            stream.str(std::wstring());
            std::wcout << stream.str();

            auto bodyStream = response.body();
            Concurrency::streams::stringstreambuf sbuffer;
            auto& target = sbuffer.collection();

            bodyStream.read_to_end(sbuffer).get();

            stream.str(std::wstring());
            std::wcout << stream.str();

            string result = target.c_str();
            return result;
        });
    }

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
    }

};

#endif