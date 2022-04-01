#include "svg.h"

namespace svg {

using namespace std::literals;

std::ostream& operator<<(std::ostream &out, StrokeLineCap line_cap) {
    switch (line_cap) {
    case StrokeLineCap::BUTT:
        out << "butt"sv;
        break;
    case StrokeLineCap::ROUND:
        out << "round"sv;
        break;
    case StrokeLineCap::SQUARE:
        out << "square"sv;
        break;
    }
    return out;
}

std::ostream& operator<<(std::ostream &out, StrokeLineJoin line_join) {
    switch (line_join) {
    case StrokeLineJoin::ARCS:
        out << "arcs"sv;
        break;
    case StrokeLineJoin::BEVEL:
        out << "bevel"sv;
        break;
    case StrokeLineJoin::MITER:
        out << "miter"sv;
        break;
    case StrokeLineJoin::MITER_CLIP:
        out << "miter-clip"sv;
        break;
    case StrokeLineJoin::ROUND:
        out << "round"sv;
        break;
    }
    return out;
}

void HtmlEncodeString(std::ostream &out, std::string_view text) {
    for (const char symbol : text) {
        if (symbol == '"') {
            out << "&quot;"sv;
        } else if (symbol == '\'') {
            out << "&apos;"sv;
        } else if (symbol == '<') {
            out << "&lt;"sv;
        } else if (symbol == '>') {
            out << "&gt;"sv;
        } else if (symbol == '&') {
            out << "&amp;"sv;
        } else {
            out << symbol;
        }
    }
}

void Object::Render(const RenderContext &context) const {
    context.RenderIndent();
    RenderObject(context);
    context.out << std::endl;
}

// ---------- ColorToOstream ---------

void ColorToOstream::operator()(std::monostate) {
    out << "none"sv;
}

void ColorToOstream::operator()(const std::string &color) {
    out << color;
}

void ColorToOstream::operator()(const Rgb &color) {
    out << "rgb("
        << +color.red << ','
        << +color.green << ','
        << +color.blue << ')';
}

void ColorToOstream::operator()(const Rgba &color) {
    out << "rgba("
        << +color.red << ','
        << +color.green << ','
        << +color.blue << ','
        << color.opacity << ')';
}

// ---------- Document ---------------

void Document::AddPtr(std::unique_ptr<Object> &&obj) {
    objects_.push_back(std::move(obj));
}

void Document::Render(std::ostream &out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
    for (const auto &obj_ptr : objects_) {
        out << ' ' << ' ';
        obj_ptr->Render(out);
    }
    out << "</svg>"sv << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center) {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius) {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext &context) const {
    auto &out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    RenderAttrs(context.out);
    out << "/>"sv;
}

// ---------- Polyline ------------------

Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext &context) const {
    auto &out = context.out;
    out << "<polyline"sv;

    out << " points=\""sv;
    bool first_point = true;
    for (const auto point : points_) {
        if (!first_point) {
            out << ' ';
        }
        out << point.x << ',' << point.y;
        first_point = false;
    }
    out << "\" "sv;
    RenderAttrs(context.out);
    out << "/>"sv;
}

// ------------ Rgb & Rgba -----------

Rgb::Rgb(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {
}

Rgba::Rgba(uint8_t r, uint8_t g, uint8_t b, double alpha) : Rgb(r, g, b), opacity(alpha) {
}

// ---------- Text ------------------

Text& Text::SetPosition(Point pos) {
    position_ = pos;
    return *this;
}

Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

Text& Text::SetFontSize(uint32_t size) {
    font_size_ = size;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = std::move(font_family);
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = std::move(font_weight);
    return *this;
}

Text& Text::SetData(std::string data) {
    data_ = std::move(data);
    return *this;
}

void Text::RenderObject(const RenderContext &context) const {
    auto &out = context.out;

    out << "<text"sv;
    RenderAttrs(context.out);
    out << " x=\""sv << position_.x << "\" y=\""sv << position_.y << "\"";
    out << " dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\"";
    out << " font-size=\"" << font_size_ << "\"";

    if (!font_family_.empty()) {
        out << " font-family=\""sv << font_family_ << "\""sv;
    }
    if (!font_weight_.empty()) {
        out << " font-weight=\""sv << font_weight_ << "\""sv;
    }

    out << ">"sv;

    HtmlEncodeString(out, data_);

    out << "</text>"sv;
}

}  // namespace svg
