#ifndef BOP_DATA_SOURCE_H
#define BOP_DATA_SOURCE_H

#include "data_source.h"
#include "hilltop_data_source.h"

using namespace std;
using namespace utility::conversions;

class bop_data_source : public hilltop_data_source {
public:
    bop_data_source() {
        _host_url = utility::conversions::to_string_t("http://ec2-52-6-196-14.compute-1.amazonaws.com/sos-bop/service");
        initiliased = false;
        data_source_name = "Bay of Plenty Regional Council";
        _source_type_dict = type_dict("Discharge_Primary", "Stage", "Water Temperature", "Precip%20Total_HourTotal");
        _source_units = units(1, 1);
    }

    void process_flow_response(string flow_res_string, std::map<string, sensor_obs> &result, observable type) {
        wcout << "flow_res_string " << flow_res_string.c_str() << endl;
        pugi::xml_document doc;
        pugi::xml_parse_result flow_response_all = doc.load_string(flow_res_string.c_str());

        pugi::xml_node responses = doc.child("sos:GetObservationResponse").child("sos:observationData").child("om:OM_Observation").child("om:result").child("wml2:MeasurementTimeseries");
        for (pugi::xml_node item : responses.children("wml2:point")) {
            pugi::xml_node feature = item.child("wml2:MeasurementTVP");
            string value = feature.child("wml2:value").text().get();
            string time = feature.child("wml2:time").text().get();
            double value_num = atof(value.c_str()) / _source_units.get_units(type);
            auto pos = result.find(time);
            sensor_obs new_flow(value_num, time, type);
            wcout << "adding flow at time " << utility::conversions::to_string_t(time).c_str() << endl;
            if (pos == result.end()) {
                result[time] = new_flow;
            }
            else {
                result[time] = pos->second + new_flow;
            }
        }
        wcout << "done adding flows " << endl;
    };
};

#endif

