#pragma once

#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "transport_catalogue.h"

namespace transport_catalogue {
namespace input {

void DataToCatalogue(std::vector<std::string>&&, router::TransportCatalogue&);

std::string Line();

int Number();

namespace detail {

void AddBus(std::string_view, router::TransportCatalogue&);

void AddStop(std::string_view, router::TransportCatalogue&);

void ParseStopDistance(std::string_view, std::string_view, router::TransportCatalogue&);

std::vector<const router::Stop*> ParseStops(std::string_view, router::TransportCatalogue&, bool &round_trip_mark);

double StringViewToDouble(std::string_view);

} // namespace detail
} //namespace input
} // namespace transport_catalogue
