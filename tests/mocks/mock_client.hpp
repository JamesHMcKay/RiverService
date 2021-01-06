#include "cpprest/http_client.h"

#include "gmock/gmock.h"

using namespace web::http::client;

class MockClient : public web::http::client::http_client {
public:

  MockClient(string url, http_client_config config) : http_client(url, config) {}

  MOCK_METHOD2(request, pplx::task<http_response>(method method, utility::string_t path_query_fragment));
};