#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "map_renderer.h"
#include "transport_catalogue.h"

namespace transport_catalogue {

class RequestHandler {
public:
    RequestHandler(router::TransportCatalogue &catalogue, map_renderer::MapRenderer &renderer);

    void AddBus(std::string_view, std::vector<const router::Stop*>&&, bool round_trip_mark);

    const router::Stop* GetStop(std::string_view) const;

    void AddStop(std::string_view, double, double);

    void SetStopDistance(std::string_view, std::string_view, int);

    std::optional<router::BusStat> GetBusStat(std::string_view) const;

    std::optional<router::StopStat> GetStopStat(std::string_view) const;

    void SetRenderSettings(map_renderer::RenderSettings &&settings);

    std::string GetMap() const;

private:
    router::TransportCatalogue &catalogue_;
    map_renderer::MapRenderer &renderer_;
};

} // namespace transport_catalogue
