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
        feature_of_interest new_feature(name, id_t, lat_lon(lat, lon), _host_url);
        result.push_back(new_feature);
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
}

void data_source::get_available_features(map<utility::string_t, feature_of_interest> &features_map) {
    if (!initiliased) {
        initiliased = true;
        get_features(features_map);
    }
}
