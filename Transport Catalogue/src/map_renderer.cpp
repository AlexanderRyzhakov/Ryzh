#include "map_renderer.h"

#include <sstream>
#include <string>

using namespace std;

namespace map_renderer {

using namespace transport_catalogue;

inline const double kEpsilon = 1e-6;

bool IsZero(double value) {
    return std::abs(value) < kEpsilon;
}

class SphereProjector {
public:
    template<typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width,
        double max_height, double padding)
    : padding_(padding) {
        if (points_begin == points_end) {
            return;
        }

        const auto [left_it, right_it]
        = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
            return lhs.lng < rhs.lng;
        });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        const auto [bottom_it, top_it]
        = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
            return lhs.lat < rhs.lat;
        });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            zoom_coeff_ = *height_zoom;
        }
    }

    svg::Point operator()(detail::Coordinates coords) const {
        return {(coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_};
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

MapRenderer::MapRenderer(const router::TransportCatalogue &catalogue) : cataolgue_ptr_(&catalogue) {
}

void MapRenderer::SetRenderSettings(RenderSettings &&settings) {
    render_settings_ = move(settings);
}

string MapRenderer::GetMap() const {
    const auto all_coordinates = GetAllCoordinates();
    SphereProjector sphere_projector(all_coordinates.begin(), all_coordinates.end(), render_settings_.width,
        render_settings_.height, render_settings_.padding);
    vector<string_view> buses_sorted = GetBusList();

    svg::Document picture;
    //add lines:
    size_t color_index = 0;
    for (size_t i = 0u; i < buses_sorted.size(); ++i) {
        const auto *bus = cataolgue_ptr_->GetBus(buses_sorted[i]);
        if (bus->stops_on_route.size() != 0) {
            picture.Add(
                GetPolylineFromRoute(bus,
                    render_settings_.color_palette[color_index++ % render_settings_.color_palette.size()],
                    sphere_projector));
        }
    }

    //add last stop names:
    color_index = 0;
    for (size_t i = 0u; i < buses_sorted.size(); ++i) {
        const auto *bus = cataolgue_ptr_->GetBus(buses_sorted[i]);
        if (bus->stops_on_route.size() != 0) {
            svg::Text stop_backround;
            svg::Text stop_main;

            const auto *stop_first = bus->stops_on_route[0];
            svg::Point position = sphere_projector(stop_first->coordinates);

            SetLastStopNameBackground(stop_backround, bus, position);

            svg::Color color = render_settings_.color_palette[color_index++ % render_settings_.color_palette.size()];
            SetLastStopNameMain(stop_main, bus, color, position);

            picture.Add(stop_backround);
            picture.Add(stop_main);

            if (!bus->round_trip) {
                const auto *stop_last = bus->last_stop;
                if (stop_first != stop_last) {
                    svg::Point position = sphere_projector(stop_last->coordinates);
                    stop_backround.SetPosition(position);
                    stop_main.SetPosition(position);

                    picture.Add(move(stop_backround));
                    picture.Add(move(stop_main));
                }
            }
        }
    }

    //add stops signs
    const auto stops_sorted = GetNonEmptySortedStops();
    for (const auto &stop : stops_sorted) {
        svg::Point position = sphere_projector(cataolgue_ptr_->GetStop(stop)->coordinates);
        svg::Circle circle;
        circle.SetCenter(position)
            .SetFillColor("white")
            .SetRadius(render_settings_.stop_radius);
        picture.Add(move(circle));
    }

    // add stops names
    for (const auto &stop : stops_sorted) {
        const auto *stop_ptr = cataolgue_ptr_->GetStop(stop);
        svg::Point position = sphere_projector(stop_ptr->coordinates);

        svg::Text stop_backround;
        svg::Text stop_main;

        SetStopNameBackground(stop_backround, stop_ptr, position);
        SetStopNameMain(stop_main, stop_ptr, position);

        picture.Add(move(stop_backround));
        picture.Add(move(stop_main));
    }

    stringstream out;
    noskipws(out);
    picture.Render(out);

    string result;
    char c;
    while (out >> c) {
        result += c;
    }

    return result;
}

std::vector<detail::Coordinates> MapRenderer::GetAllCoordinates() const {
    const auto stops = GetNonEmptySortedStops();
    vector<detail::Coordinates> result;
    result.reserve(stops.size());
    for (const auto &stop : stops) {
        result.push_back(cataolgue_ptr_->GetStop(stop)->coordinates);
    }

    return result;
}

vector<string_view> MapRenderer::GetBusList() const {
    const auto buses = cataolgue_ptr_->GetBusList();
    vector<string_view> result;
    result.reserve(buses->size());

    for (auto iter = buses->begin(); iter != buses->end(); ++iter) {
        result.push_back(iter->rute_number);
    }
    sort(result.begin(), result.end());
    return result;
}

vector<string_view> MapRenderer::GetNonEmptySortedStops() const {
    const auto stops = cataolgue_ptr_->GetStopsList();
    vector<string_view> result;
    result.reserve(stops->size());

    for (auto iter = stops->begin(); iter != stops->end(); ++iter) {
        const auto stop_stat = cataolgue_ptr_->GetStopStat(iter->name);
        if (!stop_stat->buses_for_stop->empty()) {
            result.push_back(iter->name);
        }
    }
    sort(result.begin(), result.end());
    return result;
}

svg::Polyline MapRenderer::GetPolylineFromRoute(const router::Bus *bus, svg::Color color,
    const SphereProjector &projector) const {
    svg::Polyline line;
    line.SetStrokeColor(color)
        .SetFillColor("none"s)
        .SetStrokeWidth(render_settings_.line_width)
        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    for (const auto &stop : bus->stops_on_route) {
        line.AddPoint(projector(stop->coordinates));
    }
    return line;
}

void MapRenderer::SetLastStopNameMain(svg::Text &text, const router::Bus *bus, const svg::Color &color,
    svg::Point position) const {
    SetLastStopCommonAttributes(text, bus, position);
    text.SetFillColor(color);
}

void MapRenderer::SetLastStopNameBackground(svg::Text &text, const router::Bus *bus, svg::Point position) const {
    SetLastStopCommonAttributes(text, bus, position);
    text.SetFillColor(render_settings_.underlayer_color)
        .SetStrokeColor(render_settings_.underlayer_color)
        .SetStrokeWidth(render_settings_.underlayer_width)
        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
}

void MapRenderer::SetLastStopCommonAttributes(svg::Text &text, const router::Bus *bus, svg::Point position) const {
    text.SetData(bus->rute_number)
        .SetFontSize(render_settings_.bus_label_font_size)
        .SetFontFamily("Verdana"s)
        .SetFontWeight("bold"s)
        .SetOffset( { render_settings_.bus_label_offset[0], render_settings_.bus_label_offset[1] })
        .SetPosition(position);
}

void MapRenderer::SetStopNameBackground(svg::Text &text, const router::Stop *stop, svg::Point position) const {
    SetStopNameCommonAttributes(text, stop, position);
    text.SetFillColor(render_settings_.underlayer_color)
        .SetStrokeWidth(render_settings_.underlayer_width)
        .SetStrokeColor(render_settings_.underlayer_color)
        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
}

void MapRenderer::SetStopNameMain(svg::Text &text, const router::Stop *stop, svg::Point position) const {
    SetStopNameCommonAttributes(text, stop, position);
    text.SetFillColor("black"s);
}

void MapRenderer::SetStopNameCommonAttributes(svg::Text &text, const router::Stop *stop, svg::Point position) const {
    text.SetData(stop->name)
        .SetPosition(position)
        .SetOffset( { render_settings_.stop_label_offset[0], render_settings_.stop_label_offset[1] })
        .SetFontSize(render_settings_.stop_label_font_size)
        .SetFontFamily("Verdana"s);
}

} // namespace map_renderer
