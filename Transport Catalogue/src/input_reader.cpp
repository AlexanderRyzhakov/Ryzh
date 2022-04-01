#include "input_reader.h"

#include <iomanip>
#include <iostream>
#include <charconv>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

#include "stat_reader.h"

using namespace std;

namespace transport_catalogue {
namespace input {

void DataToCatalogue(vector<string> &&requests, router::TransportCatalogue &catalogue) {
    for (const auto &line : requests) {
        string_view command = line;
        if (command.substr(0, kCommandStop.size()) == kCommandStop) {
            detail::AddStop(command, catalogue);
        }
    }
    for (const auto &line : requests) {
        string_view command = line;
        if (command.substr(0, kCommandBus.size()) == kCommandBus) {
            detail::AddBus(command, catalogue);
        }
    }
}

string Line() {
    string s;
    getline(cin, s);
    return s;
}

int Number() {
    int result = 0;
    cin >> result;
    Line();
    return result;
}

namespace detail {

void AddBus(std::string_view line, router::TransportCatalogue &catalogue) {
    router::Bus new_bus;
    line.remove_prefix(kCommandBus.size());
    auto stop_name_end_position = line.find(':');
    if (stop_name_end_position == line.npos) {
        throw std::invalid_argument("incorrect ADD BUS command");
    }
    auto rute_num = (line.substr(line.find_first_not_of(" "), stop_name_end_position));
    line.remove_prefix(stop_name_end_position + 1);
    bool round_trip_mark = false;
    auto stops = ParseStops(line, catalogue, round_trip_mark);
    catalogue.AddBus(rute_num, move(stops), round_trip_mark);
}

void AddStop(string_view line, router::TransportCatalogue &catalogue) {
    router::Stop new_stop;
    line.remove_prefix(kCommandStop.size());
    auto stop_name_end_position = line.find(':');
    if (stop_name_end_position == line.npos) {
        throw std::invalid_argument("incorrect ADD STOP command");
    }
    auto name = line.substr(0, stop_name_end_position);
    line.remove_prefix(stop_name_end_position + 1);
    auto comma_pos = line.find(',');
    if (stop_name_end_position == line.npos) {
        throw std::invalid_argument("incorrect LATITUDE format");
    }
    auto latitude = StringViewToDouble(line.substr(line.find_first_not_of(" "), comma_pos));
    line.remove_prefix(comma_pos + 1);
    auto coordinates_end = min(line.size(), line.find(','));
    auto longitude = StringViewToDouble(line.substr(line.find_first_not_of(" "), coordinates_end));

    catalogue.AddStop(name, latitude, longitude);

    if (coordinates_end != line.size()) {
        line.remove_prefix(coordinates_end + 1u);
        ParseStopDistance(name, line, catalogue);
    }
}

void ParseStopDistance(string_view stop_name, string_view line, router::TransportCatalogue &catalogue) {
    size_t separator = 0u;
    do {
        line.remove_prefix(std::min(line.find_first_not_of(" "), line.size()));
        separator = min(line.find(','), line.npos);
        int distance = 0;
        string_view next_stop = line.substr(0, separator);
        from_chars(next_stop.data(), next_stop.data() + next_stop.size(), distance);
        next_stop.remove_prefix(next_stop.find('m') + 5u);
        catalogue.SetStopDistance(stop_name, next_stop, distance);
        line.remove_prefix(separator + 1u);
    } while (separator != line.npos);
}

vector<const router::Stop*> ParseStops(string_view line, router::TransportCatalogue &catalogue, bool &round_trip_mark) {
    vector<const router::Stop*> result;
    round_trip_mark = (line.find('>') == line.npos) ? false : true;
    size_t separator = 0u;
    do {
        line.remove_prefix(std::min(line.find_first_not_of(" "), line.size()));
        separator = min(line.find('-', 0), line.find('>', 0));
        string_view stop = separator == line.npos ? line.substr(0) : line.substr(0, separator);
        result.push_back(catalogue.GetStop(stop.substr(0, stop.find_last_not_of(' ') + 1u)));
        line.remove_prefix(separator + 1u);
    } while (separator != line.npos);
    return result;
}

double StringViewToDouble(string_view line) {
    double result;
    string str = { line.data(), line.size() };
    result = stod(str);
    return result;
}

} // namespace detail
} //namespace input
} // namespace transport_catalogue
