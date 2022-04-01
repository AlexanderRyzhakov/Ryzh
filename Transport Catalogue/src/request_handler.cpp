#include "request_handler.h"

using namespace std;

namespace transport_catalogue {

RequestHandler::RequestHandler(router::TransportCatalogue &catalogue, map_renderer::MapRenderer &renderer) : catalogue_(
    catalogue), renderer_(renderer) {
}

void RequestHandler::AddBus(std::string_view bus_name, std::vector<const router::Stop*> &&stops, bool round_trip_mark) {
    catalogue_.AddBus(bus_name, move(stops), round_trip_mark);
}

void RequestHandler::AddStop(std::string_view stop_name, double latitude, double longitude) {
    catalogue_.AddStop(stop_name, latitude, longitude);
}

string RequestHandler::GetMap() const {
    return renderer_.GetMap();
}

const router::Stop* RequestHandler::GetStop(std::string_view stop_name) const {
    return catalogue_.GetStop(stop_name);
}

std::optional<router::BusStat> RequestHandler::GetBusStat(std::string_view bus_name) const {
    return catalogue_.GetBusStat(bus_name);
}

std::optional<router::StopStat> RequestHandler::GetStopStat(std::string_view stop_name) const {
    return catalogue_.GetStopStat(stop_name);
}

void RequestHandler::SetRenderSettings(map_renderer::RenderSettings &&settings) {
    renderer_.SetRenderSettings(move(settings));
}

void RequestHandler::SetStopDistance(std::string_view stop_first, std::string_view stop_second, int distance) {
    catalogue_.SetStopDistance(stop_first, stop_second, distance);
}

} // namespace transport_catalogue
