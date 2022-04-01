#include "stat_reader.h"

#include <optional>
#include <iomanip>

#include "geo.h"
#include "input_reader.h"
#include "transport_catalogue.h"

using namespace std;

namespace transport_catalogue {
namespace output {

void DataFromCatalogue(vector<string> &&requests, router::TransportCatalogue &catalogue) {
    for (const auto &line : requests) {
        string_view command = line;
        if (command.substr(0, kCommandBus.size()) == kCommandBus) {
            detail::PrintBusStat(command, catalogue);
        } else if (command.substr(0, kCommandStop.size()) == kCommandStop) {
            detail::PrintStopStat(command, catalogue);
        }
    }
}

namespace detail {

void PrintBusStat(std::string_view line, router::TransportCatalogue &catalogue) {
    cout << setprecision(6);
    cout << line << ": ";
    line.remove_prefix(kCommandBus.size());
    const auto bus = catalogue.GetBusStat(line);
    if (bus.has_value()) {
        cout << bus.value().stops_on_route << " stops on route, "
            << bus.value().unique_stops << " unique stops, "
            << bus.value().route_fact_length << " route length, "
            << bus.value().courvature << " curvature";
    } else {
        cout << "not found";
    }
    cout << '\n';
}

void PrintStopStat(std::string_view line, router::TransportCatalogue &catalogue) {
    cout << setprecision(6);
    cout << line << ":";
    line.remove_prefix(kCommandStop.size());
    const auto stop = catalogue.GetStopStat(line);
    if (stop.has_value()) {
        if (stop.value().buses_for_stop->size() == 0) {
            cout << " no buses";
        } else {
            cout << " buses";
            for (const auto bus : *stop.value().buses_for_stop) {
                cout << ' ' << bus;
            }
        }
    } else {
        cout << " not found";
    }
    cout << '\n';
}

} // namespace detail
} //namespace output
} // namespace transport_catalogue
