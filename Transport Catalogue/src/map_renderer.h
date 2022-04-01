#pragma once

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"
#include "svg.h"

using namespace transport_catalogue;
using namespace std::literals;

namespace map_renderer {

struct RenderSettings {
    RenderSettings() = default;

    double width = 600.0;
    double height = 400.0;
    double padding = 50.0;

    double line_width = 14.0;
    double stop_radius = 5.0;

    int bus_label_font_size = 20;
    std::array<double, 2> bus_label_offset = { 7.0, 15.0 };

    int stop_label_font_size = 20;
    std::array<double, 2> stop_label_offset = { 7.0, -3.0 };

    svg::Color underlayer_color = svg::Rgba { 255, 255, 255, 0.85 };
    double underlayer_width = 3.0;

    std::vector<svg::Color> color_palette { svg::Color("green"s),
        svg::Rgb(255, 160, 0), svg::Color("red"s) };
};

class SphereProjector;

class MapRenderer {
public:
    explicit MapRenderer(const router::TransportCatalogue &catalogue);

    void SetRenderSettings(RenderSettings &&settings);

    std::string GetMap() const;

private:
    std::vector<detail::Coordinates> GetAllCoordinates() const;

    std::vector<std::string_view> GetBusList() const;

    svg::Polyline GetPolylineFromRoute(const router::Bus *bus, svg::Color color,
        const SphereProjector &projector) const;

    std::vector<std::string_view> GetNonEmptySortedStops() const;

    void SetLastStopNameMain(svg::Text &text, const router::Bus *bus, const svg::Color &color,
        svg::Point position) const;

    void SetLastStopNameBackground(svg::Text &text, const router::Bus *bus, svg::Point position) const;

    void SetLastStopCommonAttributes(svg::Text &text, const router::Bus *bus, svg::Point position) const;

    void SetStopNameBackground(svg::Text &text, const router::Stop *stop, svg::Point position) const;

    void SetStopNameMain(svg::Text &text, const router::Stop *stop, svg::Point position) const;

    void SetStopNameCommonAttributes(svg::Text &text, const router::Stop *stop, svg::Point position) const;

private:
    const router::TransportCatalogue *cataolgue_ptr_;
    RenderSettings render_settings_;
};

} //namespace map_renderer

