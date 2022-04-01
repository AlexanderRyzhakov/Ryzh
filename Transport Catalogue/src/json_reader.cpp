#include "json_reader.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace transport_catalogue {

using namespace std;

void RunJsonIO(RequestHandler &handler, std::istream &input, std::ostream &output) {
    json::Document doc = json::Load(input);
    const auto requsts = doc.GetRoot().AsMap();

    const auto base_requests = requsts.find("base_requests"s);
    const auto render_settings = requsts.find("render_settings"s);
    const auto stat_requests = requsts.find("stat_requests"s);

    if (base_requests != requsts.end()) {
        std::vector<const json::Dict*> bus_attributes;
        for (const auto &request : base_requests->second.AsArray()) {
            const auto *attributes = &request.AsMap();
            const auto *request_type = &attributes->at("type");
            if (request_type->AsString() == "Stop") {
                detail::AddStop(handler, attributes);
            } else if (request_type->AsString() == "Bus") {
                bus_attributes.push_back(attributes);
            }
        }
        for (const auto &bus : bus_attributes) {
            detail::AddBus(handler, bus);
        }
    }

    if (render_settings != requsts.end()) {
        json::Dict rend_settings = render_settings->second.AsMap();
        detail::SetRenderSettings(handler, rend_settings);
    }

    json::Node result;
    if (stat_requests != requsts.end()) {
        result = detail::ObjectStatus(handler, stat_requests->second.AsArray());
    }

    json::Document found_stats(result);
    json::Print(found_stats, output);
}

namespace detail {

void AddBus(RequestHandler &handler, const json::Dict *attributes) {
    std::vector<const router::Stop*> stops_ptr;

    const auto stops_list = attributes->find("stops");
    if (stops_list != attributes->end()) {
        for (const auto &stop : stops_list->second.AsArray()) {
            stops_ptr.push_back(handler.GetStop(stop.AsString()));
        }
    }
    handler.AddBus(attributes->at("name").AsString(),
        std::move(stops_ptr),
        attributes->at("is_roundtrip").AsBool());
}

void AddStop(RequestHandler &handler, const json::Dict *attributes) {
    handler.AddStop(attributes->at("name").AsString()
        , attributes->at("latitude").AsDouble()
        , attributes->at("longitude").AsDouble());

    const auto distances_data = attributes->find("road_distances");
    if (distances_data != attributes->end()) {
        for (const auto &other_stop : distances_data->second.AsMap()) {
            handler.SetStopDistance(attributes->at("name").AsString(), other_stop.first, other_stop.second.AsInt());
        }
    }
}

svg::Color DefineColor(const json::Node &node) {
    svg::Color color;
    if (node.IsString()) {
        color = node.AsString();
    } else if (node.IsArray()) {
        const auto arr = node.AsArray();
        if (arr.size() == 3u) {
            color = svg::Rgb(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt());
        } else if (arr.size() == 4u) {
            color = svg::Rgba(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt(), arr[3].AsDouble());
        }
    } else {
        throw std::logic_error("error - parsing color failure");
    }
    return color;
}

json::Array ObjectStatus(RequestHandler &handler, const json::Array &requests) {
    json::Array results;

    for (const auto &request : requests) {
        const auto attributes = request.AsMap();
        json::Dict found_stat;
        found_stat["request_id"] = attributes.at("id");

        if (attributes.at("type").AsString() == "Bus") {
            const auto bus = handler.GetBusStat(attributes.at("name").AsString());
            if (bus.has_value()) {
                found_stat["curvature"] = static_cast<double>(bus.value().courvature);
                found_stat["route_length"] = static_cast<int>(bus.value().route_fact_length);
                found_stat["stop_count"] = static_cast<int>(bus.value().stops_on_route);
                found_stat["unique_stop_count"] = static_cast<int>(bus.value().unique_stops);
            } else {
                found_stat["error_message"] = "not found"s;
            }

        } else if (attributes.at("type").AsString() == "Stop") {
            const auto stop = handler.GetStopStat(attributes.at("name").AsString());
            if (stop.has_value()) {
                json::Array buses_for_stop;
                for (const auto &bus : *stop.value().buses_for_stop) {
                    buses_for_stop.push_back(std::string(bus.data(), bus.size()));
                }
                found_stat["buses"] = std::move(buses_for_stop);
            } else {
                found_stat["error_message"] = "not found"s;
            }

        } else if (attributes.at("type").AsString() == "Map") {
            found_stat["map"] = handler.GetMap();
        }
        results.push_back(std::move(found_stat));
    }
    return results;
}

void SetRenderSettings(RequestHandler &handler, json::Dict &settings) {
    map_renderer::RenderSettings set;

    set.bus_label_font_size = settings.at("bus_label_font_size").AsInt();
    set.height = settings.at("height").AsDouble();
    set.line_width = settings.at("line_width").AsDouble();
    set.padding = settings.at("padding").AsDouble();
    set.stop_label_font_size = settings.at("stop_label_font_size").AsInt();
    set.stop_radius = settings.at("stop_radius").AsDouble();
    set.underlayer_width = settings.at("underlayer_width").AsDouble();
    set.width = settings.at("width").AsDouble();

    set.bus_label_offset[0] = settings.at("bus_label_offset").AsArray()[0].AsDouble();
    set.bus_label_offset[1] = settings.at("bus_label_offset").AsArray()[1].AsDouble();

    set.stop_label_offset[0] = settings.at("stop_label_offset").AsArray()[0].AsDouble();
    set.stop_label_offset[1] = settings.at("stop_label_offset").AsArray()[1].AsDouble();

    set.underlayer_color = DefineColor(settings.at("underlayer_color"));

    set.color_palette.clear();
    set.color_palette.reserve(settings.at("color_palette").AsArray().size());
    for (const auto &color_node : settings.at("color_palette").AsArray()) {
        set.color_palette.push_back(DefineColor(color_node));
    }

    handler.SetRenderSettings(move(set));
}

} // namespace detail
} // namespace transport_catalogue
