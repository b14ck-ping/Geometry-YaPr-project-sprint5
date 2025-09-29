#pragma once
#include <algorithm>
#include <array>
#include <bits/ranges_algo.h>
#include <cmath>
#include <cstddef>
#include <expected>
#include <format>
#include <initializer_list>
#include <iterator>
#include <numbers>
#include <optional>
#include <print>
#include <ranges>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace geometry {

/*
 * Добавьте к методам класса Point2D и Lines2DDyn все необходимые аттрибуты и спецификаторы
 * Важно: Возвращаемый тип и принимаемые аргументы менять не нужно
 */
struct Point2D {
    double x, y;

    constexpr Point2D() : x(0), y(0) {}
    constexpr Point2D(double x, double y) : x(x), y(y) {}

    // Comparison
    bool operator<(const Point2D &other) { return x < other.x && y < other.y; }
    bool operator==(const Point2D &other) const { return x == other.x && y == other.y; }

    // Binary math operators
    Point2D operator+(const Point2D &other) const { return {x + other.x, y + other.y}; }
    Point2D operator-(const Point2D &other) const { return {x - other.x, y - other.y}; }
    Point2D operator*(double value) const { return {x * value, y * value}; }
    Point2D operator/(double value) const { return {x / value, y / value}; }

    // Binary geometry operations
    double Dot(const Point2D &other) const { return x * other.x + y * other.y; }
    double Cross(const Point2D &other) const { return x * other.y - y * other.x; }
    double Length() const { return std::sqrt(x * x + y * y); }
    double DistanceTo(const Point2D &other) const {
        Point2D tmp{x - other.x, y - other.y};
        return tmp.Length();
    }

    Point2D Normalize() const {
        const double len = Length();
        return len > 0 ? Point2D{x / len, y / len} : Point2D{0, 0};
    }
};

template <size_t N>
struct Lines2D {
    std::array<double, N> x;
    std::array<double, N> y;
};

struct Lines2DDyn {
    std::vector<double> x;
    std::vector<double> y;

    void Reserve(size_t n) {
        x.reserve(n);
        y.reserve(n);
    }
    void PushBack(Point2D p) {
        x.push_back(p.x);
        y.push_back(p.y);
    }
    void PushBack(double px, double py) {
        x.push_back(px);
        y.push_back(py);
    }
    Point2D Front() { return {x.front(), y.front()}; }
};

struct BoundingBox {
    double min_x, min_y, max_x, max_y;

    /* ваш код здесь */
    bool Overlaps(BoundingBox &other) {
        double max_left = std::max(min_x, other.min_x), min_right = std::min(max_x, other.max_x),
               max_bottom = std::max(min_y, other.min_y), min_top = std::min(max_y, other.max_y);

        if (max_left <= min_right && max_bottom <= min_top)
            return true;
        else
            return false;
    }

    double Width() const { return max_x; }

    double Height() const { return max_y; }

    Point2D Center() const { return Point2D(min_x + ((max_x - min_x) / 2), min_y + ((max_y - min_y) / 2)); }
};

struct Line {
    Point2D start, end;

    /* ваш код здесь */
    double Length() const { return std::sqrt(std::pow(start.y - end.y, 2) + std::pow(start.x - end.x, 2)); }

    Point2D Direction() const { return {end.x - start.x, end.y - start.y}; }

    BoundingBox BoundBox() const {
        return BoundingBox(std::min(start.x, end.x), std::min(start.y, end.y), std::max(start.x, end.x),
                           std::max(start.y, end.y));
    }

    double Height() const { return std::max(start.y, end.y); }

    Point2D Center() { return {(end + start) / 2.}; }
    std::array<Point2D, 2> Vertices() const { return {Point2D{start.x, start.y}, {end.x, end.y}}; }
    Lines2D<2> Lines() const { return {{start.x, end.x}, {start.y, end.y}}; }
};

struct Triangle {
    Point2D a, b, c;

    //
    // Обратите внимание! В методе Lines(), в отличие от Vertices(), координаты точек замыкаются на начало:
    // a.x, b.x, c.x а затем идёт снова первая вершина a.x
    //
    // Это необходимо для правильного рисования фигур через gnuplot, который формирует линии используя пары точек.
    // В случае с  Triangle будут составлены такие пары точек:
    //      - { a, b }
    //      - { b, c }
    //      - { c, a }
    //
    Point2D Center() { return {(a + b + c) / 3.0}; }
    std::array<Point2D, 3> Vertices() const { return {a, b, c}; }
    Lines2D<4> Lines() const { return {{a.x, b.x, c.x, a.x}, {a.y, b.y, c.y, a.y}}; }

    /* ваш код здесь */
    double Area() const { return std::abs(a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) / 2.0; }

    double Height() const { return std::max({a.y, b.y, c.y}); }

    BoundingBox BoundBox() const {
        return BoundingBox{std::min({a.x, b.x, c.x}), std::min({a.y, b.y, c.y}), std::max({a.x, b.x, c.x}),
                           std::max({a.y, b.y, c.y})};
    }
};

struct Rectangle {
    Point2D bottom_left;
    double width, height;

    /* ваш код здесь */
    Point2D Center() const {
        return Line{{bottom_left.x, bottom_left.y}, {bottom_left.x + width, bottom_left.y + height}}.Center();
    }

    std::array<Point2D, 4> Vertices() const {
        return {Point2D{bottom_left.x, bottom_left.y},
                {bottom_left.x, bottom_left.y + height},
                {bottom_left.x + width, bottom_left.y + height},
                {bottom_left.x + width, bottom_left.y}};
    }
    Lines2D<5> Lines() const {
        return {{bottom_left.x, bottom_left.x, bottom_left.x + width, bottom_left.x + width, bottom_left.x},
                {bottom_left.y, bottom_left.y + height, bottom_left.y + height, bottom_left.y, bottom_left.y}};
    }

    double Area() const { return width * height; }

    double Height() const { return bottom_left.y + height; }

    BoundingBox BoundBox() const {
        return BoundingBox{bottom_left.x, bottom_left.y, bottom_left.x + width, bottom_left.y + height};
    }
};

struct RegularPolygon {
    Point2D center_p;
    double radius;
    int sides;

    constexpr RegularPolygon(Point2D center, double radius, int sides)
        : center_p(center), radius(radius), sides(sides) {}

    Point2D Center() const { return center_p; }

    std::vector<Point2D> Vertices() const {
        std::vector<Point2D> points;
        points.reserve(sides);

        for (int i = 0; i < sides; ++i) {
            const double angle = 2 * std::numbers::pi * i / sides;
            points.emplace_back(center_p.x + radius * std::cos(angle), center_p.y + radius * std::sin(angle));
        }
        return points;
    }

    Lines2DDyn Lines() const {
        Lines2DDyn lines{};
        lines.Reserve(sides);

        for (int i = 0; i < sides; ++i) {
            const double angle = 2 * std::numbers::pi * i / sides;
            lines.PushBack(center_p.x + radius * std::cos(angle), center_p.y + radius * std::sin(angle));
        }
        if (sides > 2)
            lines.PushBack(center_p.x + radius * std::cos(0), center_p.y + radius * std::sin(0));

        return lines;
    }

    double Height() const {
        double max_y = center_p.y + radius * std::sin(0);
        for (int i = 1; i < sides; ++i) {
            const double angle = 2 * std::numbers::pi * i / sides;
            double y = center_p.y + radius * std::sin(angle);
            if (y > max_y)
                max_y = y;
        }
        return max_y;
    }

    BoundingBox BoundBox() const {
        Point2D max{};
        Point2D min{};

        for (int i = 0; i < sides; ++i) {
            const double angle = 2 * std::numbers::pi * i / sides;
            double x = center_p.x + radius * std::cos(angle);
            double y = center_p.y + radius * std::sin(angle);
            max.x = std::max(max.x, x);
            max.y = std::max(max.y, y);
            min.x = std::min(min.x, x);
            min.y = std::min(min.y, y);
        }
        return BoundingBox{min.x, min.y, max.x, max.y};
    }
};

struct Circle {
    Point2D center_p;
    double radius;

    constexpr Circle(Point2D center, double radius) : center_p(center), radius(radius) {}

    BoundingBox BoundBox() const {
        return {center_p.x - radius, center_p.y - radius, center_p.x + radius, center_p.y + radius};
    }
    double Height() const { return center_p.y + radius; }
    Point2D Center() const { return center_p; }

    std::vector<Point2D> Vertices(size_t N = 30) const {
        std::vector<Point2D> points;
        points.reserve(N);

        for (int i = 0; i < N; ++i) {
            const double angle = 2 * std::numbers::pi * i / N;
            points.emplace_back(center_p.x + radius * std::cos(angle), center_p.y + radius * std::sin(angle));
        }
        return points;
    }
    Lines2DDyn Lines(size_t N = 100) const {
        Lines2DDyn lines{};
        lines.Reserve(N);

        for (int i = 0; i < N; ++i) {
            const double angle = 2 * std::numbers::pi * i / N;
            lines.PushBack(center_p.x + radius * std::cos(angle), center_p.y + radius * std::sin(angle));
        }
        if (N > 2)
            lines.PushBack(center_p.x + radius * std::cos(0), center_p.y + radius * std::sin(0));

        return lines;
    }
};

class Polygon {
public:
    constexpr Polygon(std::initializer_list<Point2D> list) {
        points_ = list;
        get_bounding_box();
    }

    constexpr Polygon(const std::vector<Point2D> &vec) {
        points_ = vec;
        get_bounding_box();
    }

    Polygon(std::vector<Point2D> &&vec) {
        points_ = std::move(vec);
        get_bounding_box();
    }

    BoundingBox BoundBox() const { return bounding_box_; }

    double Height() const {
        return std::ranges::max(points_, [](const Point2D &a, const Point2D &b) { return a.y < b.y; }).y;
    }

    Point2D Center() const {
        Point2D center = std::ranges::fold_left(points_, {}, std::plus<>{});
        return center / static_cast<double>(std::ranges::distance(points_));
    }

    std::vector<Point2D> Vertices() const { return points_; }

    Lines2DDyn Lines() const {
        Lines2DDyn lines{};
        lines.Reserve(std::ranges::distance(points_));

        std::ranges::for_each(points_, [&lines](const Point2D &p) { lines.PushBack(p); });
        lines.PushBack(points_.at(0));

        return lines;
    }

private:
    constexpr void get_bounding_box() {
        Point2D p_min{points_[0]}, p_max{points_[0]};

        std::ranges::for_each(points_, [&p_min, &p_max](const Point2D &a) {
            p_min.x = std::min(p_min.x, a.x);
            p_min.y = std::min(p_min.y, a.y);
            p_max.x = std::max(p_max.x, a.x);
            p_max.y = std::max(p_max.y, a.y);
        });
        bounding_box_ = {p_min.x, p_min.y, p_max.x, p_max.y};
    }

    std::vector<Point2D> points_;
    BoundingBox bounding_box_;
};

using Shape = std::variant<Line, Triangle, Rectangle, RegularPolygon, Circle, Polygon>;

enum class GeometryError { Unsupported, NoIntersection, InvalidInput, DegenrateCase, InsufficientPoints };

template <typename T>
using GeometryResult = std::expected<T, GeometryError>;
}  // namespace geometry

template <>
struct std::formatter<geometry::Point2D> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Point2D &p, FormatContext &ctx) const {
        return format_to(ctx.out(), "({:.2f}, {:.2f})", p.x, p.y);
    }
};

template <>
struct std::formatter<std::vector<geometry::Point2D>> {
    mutable bool use_new_line = false;

    constexpr auto parse(std::format_parse_context &ctx) const -> decltype(ctx.begin()) {
        auto it = ctx.begin();
        auto end = ctx.end();

        if (it == end || *it == '}')
            return it;

        auto start = it;
        while (it != end && *it != '}')
            ++it;

        std::string_view spec(start, it);
        if (spec == "new_line") {
            use_new_line = true;
        }

        return it;
    }

    template <typename FormatContext>
    auto format(const std::vector<geometry::Point2D> &v, FormatContext &ctx) const {
        bool new_line = use_new_line;

        if (v.empty())
            return ctx.out();

        std::string result{};
        auto to_str = [&](const geometry::Point2D &p) {
            result += (new_line ? "\n\t" : ", ") + "{"s + std::format("{:.1f}", p.x) + ", "s +
                      std::format("{:.1f}", p.y) + "}"s;
        };

        result = (new_line ? "\t" : "") + "{"s + std::format("{:.1f}", v[0].x) + ", "s + std::format("{:.1f}", v[0].y) +
                 "}"s;
        std::ranges::for_each(std::ranges::begin(v) + 1, std::ranges::end(v), to_str);

        return std::format_to(ctx.out(), "{}", result);
    }
};

template <>
struct std::formatter<geometry::Line> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Line &l, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Line({}, {})", l.start, l.end);
    }
};

template <>
struct std::formatter<geometry::Circle> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Circle &c, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Circle(center={}, r={:.2f})", c.center_p, c.radius);
    }
};

template <>
struct std::formatter<geometry::Rectangle> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Rectangle &r, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Rectangle(bottom_left={}, w={:.2f}, h={:.2f})", r.bottom_left, r.width,
                              r.height);
    }
};

template <>
struct std::formatter<geometry::RegularPolygon> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::RegularPolygon &p, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "RegularPolygon(center={}, r={:.2f}, sides={})", p.center_p, p.radius,
                              p.sides);
    }
};
template <>
struct std::formatter<geometry::Triangle> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Triangle &t, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Triangle({}, {}, {})", t.a, t.b, t.c);
    }
};
template <>
struct std::formatter<geometry::Polygon> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Polygon &poly, FormatContext &ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "Polygon[{} points]: [", poly.Vertices().size());

        for (const auto &p : poly.Vertices()) {
            out = std::format_to(out, "{} ", p);
        }

        return std::format_to(out, "]");
    }
};
