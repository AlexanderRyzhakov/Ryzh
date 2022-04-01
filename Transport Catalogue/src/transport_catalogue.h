#pragma once

#include <deque>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "geo.h"

namespace transport_catalogue {

const std::string kCommandBus = "Bus ";
const std::string kCommandStop = "Stop ";

namespace router {

struct Stop {
    std::string name;
    detail::Coordinates coordinates;

    bool operator==(const Stop&) const;

    bool operator<(const Stop&) const;
};

struct StopStat {
    std::string_view name;
    const std::set<std::string_view> *buses_for_stop;
};

class PairStopsHasher {
public:
    size_t operator()(const std::pair<const Stop*, const Stop*>) const;

private:
    const int prime_ = 37;
    std::hash<Stop*> hasher_;
};

struct Bus {
    Bus() = default;

    std::string rute_number;
    std::vector<const Stop*> stops_on_route;
    std::unordered_set<const Stop*> unique_stops;
    const Stop *last_stop = nullptr;
    bool round_trip = false;

    bool operator==(const Bus&) const;

    bool operator<(const Bus&) const;
};

struct BusStat {
    std::string_view route;

    size_t stops_on_route = 0;
    size_t unique_stops = 0;

    uint32_t route_fact_length = 0;
    double route_shortest_length = .0;
    double courvature = .0;
};

class TransportCatalogue {
public:
    void AddBus(std::string_view, std::vector<const Stop*>&&, bool round_trip_mark);

    void AddStop(std::string_view, double, double);

    const Bus* GetBus(std::string_view) const;

    const std::deque<Bus>* GetBusList() const;

    std::optional<BusStat> GetBusStat(std::string_view) const;

    int GetDistance(const Stop*, const Stop*) const;

    const Stop* GetStop(std::string_view) const;

    const std::deque<Stop>* GetStopsList() const;

    std::optional<StopStat> GetStopStat(std::string_view) const;

    void SetStopDistance(std::string_view, std::string_view, int);

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;

    std::unordered_map<std::string_view, const Stop*> stops_ptr_;
    std::unordered_map<std::string_view, const Bus*> buses_ptr_;

    std::unordered_map<const Stop*, std::set<std::string_view>> stops_to_buses_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, PairStopsHasher> stops_distance_;
};

} //namespace router
} //namespace transport_catalogue
