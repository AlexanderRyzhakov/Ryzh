#pragma once

#include "json.h"
#include "request_handler.h"

namespace transport_catalogue {

void RunJsonIO(RequestHandler &handler, std::istream &input, std::ostream &output = std::cout);

namespace detail {

void AddBus(RequestHandler &handler, const json::Dict *attributes);

void AddStop(RequestHandler &handler, const json::Dict *attributes);

svg::Color DefineColor(const json::Node &node);

json::Array ObjectStatus(RequestHandler &handlere, const json::Array &requests);

void SetRenderSettings(RequestHandler &handler, json::Dict &settings);

} // namespace detail
} // namespace transport_catalogue
