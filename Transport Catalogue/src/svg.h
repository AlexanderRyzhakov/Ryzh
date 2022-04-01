#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace svg {

class Rgb;
class Rgba;

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

inline const std::string kNoneColor { "none" };

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

std::ostream& operator<<(std::ostream &out, StrokeLineCap line_cap);

std::ostream& operator<<(std::ostream &out, StrokeLineJoin line_join);

void HtmlEncodeString(std::ostream &out, std::string_view text);

struct ColorToOstream {
    void operator()(std::monostate);

    void operator()(const std::string &color);

    void operator()(const Rgb &color);

    void operator()(const Rgba &color);

    std::ostream &out;
};

struct Point {
    Point() = default;
    Point(double x, double y) : x(x), y(y) {
    }
    double x = 0;
    double y = 0;
};

struct RenderContext {
    RenderContext(std::ostream &out) : out(out) {
    }

    RenderContext(std::ostream &out, int indent_step, int indent = 0)
    : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream &out;
    int indent_step = 0;
    int indent = 0;
};

struct Rgb {
    Rgb() = default;

    Rgb(uint8_t r, uint8_t g, uint8_t b);

    uint8_t red = 0u;
    uint8_t green = 0u;
    uint8_t blue = 0u;
};

struct Rgba: Rgb {
    Rgba() = default;

    Rgba(uint8_t r, uint8_t g, uint8_t b, double alpha);

    double opacity = 1.0;
};

template<typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeWidth(double width) {
        width_ = width;
        return AsOwner();
    }

    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        line_cap_ = line_cap;
        return AsOwner();
    }

    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        line_join_ = line_join;
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream &out) const {
        using namespace std::literals;

        if (fill_color_) {
            out << " fill=\""sv;
            std::visit(ColorToOstream { out }, *fill_color_);
            out << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv;
            std::visit(ColorToOstream { out }, *stroke_color_);
            out << "\""sv;
        }
        if (width_) {
            out << " stroke-width=\""sv << *width_ << "\""sv;
        }
        if (line_cap_) {
            //stroke-linecap="round"
            out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
        }
        if (line_join_) {
            out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
        }
    }

private:
    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }

private:
    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;
};

class Object {
public:
    virtual ~Object() = default;

    void Render(const RenderContext &context) const;

protected:
    virtual void RenderObject(const RenderContext &context) const = 0;
};

class ObjectContainer {
public:
    virtual ~ObjectContainer() = default;

    template<typename ObjectChild>
    void Add(ObjectChild object_child);

    virtual void AddPtr(std::unique_ptr<Object> &&obj) = 0;
};

class Drawable {
public:
    virtual ~Drawable() = default;

    virtual void Draw(ObjectContainer &obj_container) const = 0;
};

// https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext &context) const override;

    Point center_;
    double radius_ = 1.0;
};

// https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
class Polyline final : public Object, public PathProps<Polyline> {
public:
    Polyline& AddPoint(Point point);

private:
    void RenderObject(const RenderContext &context) const override;

private:
    std::vector<Point> points_;
};

// https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
class Text final: public Object, public PathProps<Text> {
public:
    Text& SetPosition(Point pos);

    Text& SetOffset(Point offset);

    Text& SetFontSize(uint32_t size);

    Text& SetFontFamily(std::string font_family);

    Text& SetFontWeight(std::string font_weight);

    Text& SetData(std::string data);

private:
    void RenderObject(const RenderContext &context) const override;

private:
    Point position_;
    Point offset_;
    uint32_t font_size_ = 1u;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;
};

class Document: public ObjectContainer {
public:
    Document() = default;

    void AddPtr(std::unique_ptr<Object> &&obj);

    void Render(std::ostream &out) const;

private:
    std::vector<std::unique_ptr<Object>> objects_;
};

//----------template methods implementations:

template<typename ObjectChild>
void ObjectContainer::Add(ObjectChild object_child) {
    AddPtr(std::make_unique<ObjectChild>(std::move(object_child)));
}

}  // namespace svg
