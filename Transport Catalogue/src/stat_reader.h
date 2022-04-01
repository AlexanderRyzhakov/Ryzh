#pragma once

#include <iostream>
#include <optional>

#include "transport_catalogue.h"

namespace transport_catalogue {
namespace output {

void DataFromCatalogue(std::vector<std::string>&&, router::TransportCatalogue&);

namespace detail {

void PrintBusStat(std::string_view, router::TransportCatalogue&);

void PrintStopStat(std::string_view, router::TransportCatalogue&);

} //namespace detail
} //namespace output
} // namespace transport_catalogue
