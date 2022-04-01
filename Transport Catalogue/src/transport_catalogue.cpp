#include "transport_catalogue.h"

#include <stdexcept>
#include <string_view>
#include <utility>

#include "geo.h"

using namespace std;
namespace transport_catalogue {
namespace router {

bool Bus::operator==(const Bus &other) const {
    return rute_number == other.rute_number;
}

bool Bus::operator<(const Bus &other) const {
    return rute_number < other.rute_number;
}

bool Stop::operator==(const Stop &other) const {
    return name == other.name;
}

bool Stop::operator<(const Stop &other) const {
    return name < other.name;
}

size_t PairStopsHasher::operator()(const std::pair<const Stop*, const Stop*> pair_stop) const {
    return hasher_(const_cast<Stop*>(pair_stop.first)) + hasher_(const_cast<Stop*>(pair_stop.second)) * prime_;
}

//Transport_catalogue methods:
//Adders information to catalogue (modifying):
void TransportCatalogue::AddBus(std::string_view rute_number, std::vector<const Stop*> &&stops, bool round_trip_mark) {
    Bus new_bus;
    new_bus.rute_number = { rute_number.data(), rute_number.size() };
    new_bus.round_trip = round_trip_mark;
    new_bus.stops_on_route = move(stops);
    auto &bus_ptr = buses_.emplace_back(move(new_bus));
    for (const auto stop : bus_ptr.stops_on_route) {
        bus_ptr.unique_stops.emplace(stop);
        stops_to_buses_[stop].emplace(bus_ptr.rute_number);
    }
    if (!round_trip_mark) {
        bus_ptr.last_stop = *bus_ptr.stops_on_route.rbegin();
        int last_stop_pos = bus_ptr.stops_on_route.size() - 1;
        for (int i = last_stop_pos - 1; i >= 0; --i) {
            bus_ptr.stops_on_route.push_back(bus_ptr.stops_on_route[i]);
        }
    }
    buses_ptr_[bus_ptr.rute_number] = &bus_ptr;
}

void TransportCatalogue::AddStop(std::string_view name, double latitude, double longitude) {
    if (auto added_stop = GetStop(name)) {
        const_cast<Stop*>(added_stop)->coordinates.lat = latitude;
        const_cast<Stop*>(added_stop)->coordinates.lng = longitude;
    } else {
        Stop new_stop;
        new_stop.name = { name.data(), name.size() };
        new_stop.coordinates.lat = latitude;
        new_stop.coordinates.lng = longitude;
        auto &stop_ptr = stops_.emplace_back(move(new_stop));
        stops_ptr_[stop_ptr.name] = &stop_ptr;
        stops_to_buses_[&stop_ptr];
    }
}

void TransportCatalogue::SetStopDistance(std::string_view stop_first, std::string_view stop_second, int distance) {
    if (stops_ptr_.find(stop_second) == stops_ptr_.end()) {
        AddStop(stop_second, .0, .0);
    }
    stops_distance_[make_pair(stops_ptr_.at(stop_first), stops_ptr_.at(stop_second))] = distance;
}

//get pointers to real subjects (non-modifying):
const Bus* TransportCatalogue::GetBus(string_view route) const {
    const auto bus = buses_ptr_.find(route);
    return bus != buses_ptr_.end() ? bus->second : nullptr;
}

const Stop* TransportCatalogue::GetStop(string_view stop_name) const {
    const auto stop = stops_ptr_.find(stop_name);
    return stop != stops_ptr_.end() ? stop->second : nullptr;
}

const std::deque<Bus>* TransportCatalogue::GetBusList() const {
    return &buses_;
}

const std::deque<Stop>* TransportCatalogue::GetStopsList() const {
    return &stops_;
}

//information getters (non-modifying):
optional<BusStat> TransportCatalogue::GetBusStat(string_view route) const {
    auto bus_ptr = GetBus(route);
    if (bus_ptr) {
        BusStat stat;

        stat.route = bus_ptr->rute_number;
        stat.stops_on_route = bus_ptr->stops_on_route.size();
        stat.unique_stops = bus_ptr->unique_stops.size();

        auto *stops_for_bus_ptr = &bus_ptr->stops_on_route;

        for (size_t i = 1; i < stops_for_bus_ptr->size(); ++i) {
            auto stop_a = stops_for_bus_ptr->operator [](i - 1);
            auto stop_b = stops_for_bus_ptr->operator [](i);
            stat.route_shortest_length += ComputeDistance(stop_a->coordinates, stop_b->coordinates);
            stat.route_fact_length += GetDistance(stop_a, stop_b);
        }
        stat.courvature = stat.route_fact_length / stat.route_shortest_length;
        return stat;
    }
    return nullopt;
}

int TransportCatalogue::GetDistance(const Stop *stop_a, const Stop *stop_b) const {
    auto straight = stops_distance_.find(make_pair(stop_a, stop_b));
    auto backward = stops_distance_.find(make_pair(stop_b, stop_a));

    if (straight != stops_distance_.end()) {
        return straight->second;
    } else if (backward == stops_distance_.end()) {
        throw std::invalid_argument("no stop connection for bus");
    }
    return backward->second;
}

optional<StopStat> TransportCatalogue::GetStopStat(string_view stop_name_request) const {
    auto stop_ptr = GetStop(stop_name_request);
    if (stop_ptr) {
        StopStat stop;

        stop.name = stop_ptr->name;
        stop.buses_for_stop = &stops_to_buses_.at(stop_ptr);
        return stop;
    }
    return nullopt;
}

} //namespace router
} //namespace transport_catalogue
