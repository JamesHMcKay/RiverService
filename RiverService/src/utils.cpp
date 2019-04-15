#include "utils.h"

using namespace std;
using namespace Concurrency;
using namespace chrono;

namespace utils {
    string ref_time_str() {
        return "2019-04-14T01:00:00.000Z";
    }

    time_t my_timegm(tm* _Tm)
    {
        auto t = mktime(_Tm);
        return t + (mktime(localtime(&t)) - mktime(gmtime(&t)));
    }

    chrono::system_clock::time_point convert_time_str(string time) {
        std::istringstream input(time);
        tm t{};
        input >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
        return chrono::system_clock::from_time_t(my_timegm(&t));
    }

    // fix this to get the actual date time
    string get_distant_future_time() {
        return "2100-06-01T00:00:00.000Z";
    }

    string get_time_utc(chrono::system_clock::time_point time_now, string time_zone_diff) {
        int offset = std::stoi(time_zone_diff);
        time_now += -chrono::hours(offset) + chrono::seconds(1);
        // figure out what is going on with this to_time_t function
        auto time_t_now = std::chrono::system_clock::to_time_t(time_now);
        std::ostringstream ss;
        ss << std::put_time(gmtime(&time_t_now), "%FT%TZ");
        return ss.str();
    }

    system_clock::time_point get_time_point_utc(string time_str) {
        int time_str_length = time_str.size();
        string time_zone = time_str.substr(time_str_length - 5, time_str_length - 1);
        system_clock::time_point first_time = utils::convert_time_str(time_str);
        int offset = std::stoi(time_zone);
        first_time += -chrono::hours(offset);
        return first_time;
    }
}
