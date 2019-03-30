#include "data_source.h"

using namespace std;
using namespace Concurrency;


void data_source::process_feature_response(pugi::xml_node responses, std::vector<feature_of_interest> &result) {
    //std::cout << "processing responses" << endl;
    for (pugi::xml_node item : responses.children("sos:featureMember"))
    {
        pugi::xml_node feature = item.child("sams:SF_SpatialSamplingFeature");
        string name = feature.child("gml:name").text().get();
        string id = feature.child("gml:identifier").text().get();
        string position = feature.child("sams:shape").child("ns:Point").child("ns:pos").text().get();

        utility::string_t id_t = utility::conversions::to_string_t(id);
        //cout << "got feature with id = " << id << endl;

        std::vector<std::string> position_element;
        boost::split(position_element, position, [](char c) {return c == ' '; });
        double lat = atof(position_element[0].c_str());
        double lon = atof(position_element[1].c_str());
        feature_of_interest new_feature(name, id_t, lat_lon(lat, lon));
        result.push_back(new_feature);
    }
}

void data_source::process_flow_response(pugi::xml_node responses, std::vector<sensor_obs> &result) {
    //std::cout << "processing flows" << endl;
    for (pugi::xml_node item : responses.children("wml2:point"))
    {
        pugi::xml_node feature = item.child("wml2:MeasurementTVP");
        string value = feature.child("wml2:value").text().get();
        string time = feature.child("wml2:time").text().get();
        utility::string_t time_t = utility::conversions::to_string_t(time);

        double value_num = atof(value.c_str());
        sensor_obs new_flow(value_num, time_t, "units");
        result.push_back(new_flow);
    }
}

pplx::task<string> data_source::get_features()
{
    http_client client(U("https://hydro-sos.niwa.co.nz/"));
    uri_builder builder;
    builder.append_query(U("service"), U("SOS"));
    builder.append_query(U("version"), U("2.0.0"));
    builder.append_query(U("request"), U("GetFeatureOfInterest"));
    auto path_query_fragment = builder.to_string();

    return client.request(methods::GET, path_query_fragment).then([](http_response response)
    {
        std::wostringstream stream;
        //stream << L"Server returned returned status code " << response.status_code() << L'.' << std::endl;
        std::wcout << stream.str();

        stream.str(std::wstring());
        //stream << L"Content type: " << response.headers().content_type() << std::endl;
        //stream << L"Content length: " << response.headers().content_length() << L"bytes" << std::endl;
        std::wcout << stream.str();

        auto bodyStream = response.body();
        streams::stringstreambuf sbuffer;
        auto& target = sbuffer.collection();

        bodyStream.read_to_end(sbuffer).get();

        stream.str(std::wstring());
        std::wcout << stream.str();

        string result = target.c_str();
        return result;
    });
}


pplx::task<string> data_source::get_flow_data(utility::string_t feature_id)
{
    string_t url = utility::conversions::to_string_t(_host_url);
    http_client client(url);
    uri_builder builder;
    builder.append_query(U("service"), U("SOS"));
    builder.append_query(U("version"), U("2.0.0"));
    builder.append_query(U("request"), U("GetObservation"));
    builder.append_query(U("FeatureOfInterest"), feature_id);
    builder.append_query(U("ObservedProperty"), U("Discharge"));
    builder.append_query(U("TemporalFilter"), U("om:phenomenonTime,2014-01-03T12:00/2014-01-10T12:00"));
    auto path_query_fragment = builder.to_string();

    return client.request(methods::GET, path_query_fragment).then([](http_response response)
    {
        std::wostringstream stream;
        //stream << L"Server returned returned status code " << response.status_code() << L'.' << std::endl;
        std::wcout << stream.str();

        stream.str(std::wstring());
        //stream << L"Content type: " << response.headers().content_type() << std::endl;
        //stream << L"Content length: " << response.headers().content_length() << L"bytes" << std::endl;
        std::wcout << stream.str();

        auto bodyStream = response.body();
        streams::stringstreambuf sbuffer;
        auto& target = sbuffer.collection();

        bodyStream.read_to_end(sbuffer).get();

        stream.str(std::wstring());
        std::wcout << stream.str();

        string result = target.c_str();
        return result;
    });
}

void data_source::get_flows(feature_of_interest &feature) {
    // get an example flow response
    std::vector<sensor_obs> flows;

    string flow_res_string = get_flow_data(feature.get_id()).get();
    pugi::xml_document doc;
    pugi::xml_parse_result flow_response_all = doc.load_string(flow_res_string.c_str());

    pugi::xml_node flow_responses = doc.child("sos:GetObservationResponse").child("sos:observationData").child("om:OM_Observation").child("om:result").child("wml2:MeasurementTimeseries");

    wcout << "got responses" << endl;
    process_flow_response(flow_responses, flows);

    //for (unsigned int i = 0; i < flows.size(); i++) {
        //cout << "response number " << i << endl;
    //    flows[i].print_content();
    //}
    feature.add_sensor_obs(flows);
}

void data_source::get_features(map<utility::string_t, feature_of_interest> &features_map) {
    std::vector<feature_of_interest> features;
    std::wcout << L"Calling HTTPGetAsync..." << std::endl;
    string res_string = get_features().get();
    pugi::xml_document doc;
    pugi::xml_parse_result response_all = doc.load_string(res_string.c_str());

    pugi::xml_node responses = doc.child("sos:GetFeatureOfInterestResponse");

    wcout << "got responses" << endl;
    process_feature_response(responses, features);
    for (unsigned int i = 0; i < features.size(); i++) {
        feature_of_interest next_feature = features[i];
        features_map[next_feature.get_id()] = next_feature;
        wcout << "got feature id = " << next_feature.get_id().c_str() << endl;
    }
    get_flows(features_map[utility::conversions::to_string_t("3722")]);
}

void data_source::update(map<utility::string_t, feature_of_interest> &features_map) {
    cout << "!initialised = " << !initiliased;
    if (!initiliased) {
        initiliased = true;
        cout << "set !initialsed to  = " << !initiliased << endl;
        get_features(features_map);
    }
}
