#pragma once
#include "geometry.hpp"
#include <algorithm>
#include <expected>
#include <format>
#include <initializer_list>
#include <map>
#include <vector>

namespace geometry::triangulation {

struct DelaunayTriangle {
    Point2D a, b, c;

    DelaunayTriangle(Point2D a, Point2D b, Point2D c) : a(a), b(b), c(c) {}

    bool ContainsPoint(const Point2D &p) const {
        Point2D center = Circumcenter();
        double radius = Circumradius();
        return center.DistanceTo(p) < radius + 1e-10;
    }

    Point2D Circumcenter() const {
        double d = 2 * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));
        if (std::abs(d) < 1e-10) {
            return {(a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3};
        }

        double ux = ((a.x * a.x + a.y * a.y) * (b.y - c.y) + (b.x * b.x + b.y * b.y) * (c.y - a.y) +
                     (c.x * c.x + c.y * c.y) * (a.y - b.y)) /
                    d;

        double uy = ((a.x * a.x + a.y * a.y) * (c.x - b.x) + (b.x * b.x + b.y * b.y) * (a.x - c.x) +
                     (c.x * c.x + c.y * c.y) * (b.x - a.x)) /
                    d;

        return {ux, uy};
    }

    double Circumradius() const {
        Point2D center = Circumcenter();
        return center.DistanceTo(a);
    }

    bool SharesEdge(const DelaunayTriangle &other) const {
        std::vector<Point2D> this_points = {a, b, c};
        std::vector<Point2D> other_points = {other.a, other.b, other.c};

        int shared_count = 0;
        for (const Point2D &p1 : this_points) {
            for (const Point2D &p2 : other_points) {
                if (std::abs(p1.x - p2.x) < 1e-10 && std::abs(p1.y - p2.y) < 1e-10) {
                    shared_count++;
                    break;
                }
            }
        }

        return shared_count == 2;
    }

    std::vector<Point2D> vertices() const { return {a, b, c}; }

    bool operator==(const DelaunayTriangle &other) const { return a == other.a && b == other.b && c == other.c; }
};

struct Edge {
    Point2D p1, p2;

    Edge(Point2D p1, Point2D p2) : p1(p1), p2(p2) {
        if (p1.x > p2.x || (p1.x == p2.x && p1.y > p2.y)) {
            std::swap(this->p1, this->p2);
        }
    }

    bool operator<(const Edge &other) const {
        if (std::abs(p1.x - other.p1.x) > 1e-10)
            return p1.x < other.p1.x;
        if (std::abs(p1.y - other.p1.y) > 1e-10)
            return p1.y < other.p1.y;
        if (std::abs(p2.x - other.p2.x) > 1e-10)
            return p2.x < other.p2.x;
        return p2.y < other.p2.y;
    }

    bool operator==(const Edge &other) const {
        return std::abs(p1.x - other.p1.x) < 1e-10 && std::abs(p1.y - other.p1.y) < 1e-10 &&
               std::abs(p2.x - other.p2.x) < 1e-10 && std::abs(p2.y - other.p2.y) < 1e-10;
    }
};

inline GeometryResult<std::vector<DelaunayTriangle>> DelaunayTriangulation(const std::span<Point2D> points) {

    if (points.size() < 3) {
        return std::unexpected(GeometryError::InvalidInput);
    }

    geometry::Polygon poly{std::vector<Point2D>(points.begin(), points.end())};
    auto bb = poly.BoundBox();

    double padding = std::max(bb.max_x - bb.min_x, bb.max_y - bb.min_y) * 1.0;
    Point2D super1{bb.min_x - padding, bb.min_y - padding};
    Point2D super2{bb.max_x + padding * 2, bb.min_y - padding};
    Point2D super3{bb.min_x + padding, bb.max_y + padding * 2};
    std::vector<DelaunayTriangle> triangulation;
    triangulation.emplace_back(super1, super2, super3);

    auto watson_bowyer_algo = [&triangulation](const Point2D &point) {
        std::vector<DelaunayTriangle> bad_tr = triangulation |
                                               std::ranges::views::filter([&point](const DelaunayTriangle &tr) -> bool {
                                                   return tr.ContainsPoint(point);
                                               }) |
                                               std::ranges::to<std::vector>();
        if (bad_tr.empty())
            return;

        std::vector<Edge> all_edges;
        std::ranges::for_each(bad_tr, [&all_edges](const auto &triangle) {
            auto vertices = triangle.vertices();
            all_edges.emplace_back(vertices[0], vertices[1]);
            all_edges.emplace_back(vertices[1], vertices[2]);
            all_edges.emplace_back(vertices[2], vertices[0]);
        });

        std::map<Edge, int> edge_count;
        std::ranges::for_each(all_edges, [&edge_count](const auto &edge) { edge_count[edge]++; });

        // Find boundary edges (edges that appear only once)
        auto boundary_edges = edge_count | std::views::filter([](const auto &pair) { return pair.second == 1; }) |
                              std::views::transform([](const auto &pair) { return pair.first; }) |
                              std::ranges::to<std::vector>();

        auto it = std::ranges::remove_if(triangulation, [&bad_tr](const DelaunayTriangle &tri) {
            return std::find(bad_tr.begin(), bad_tr.end(), tri) != bad_tr.end();
        });

        triangulation.erase(it.begin(), it.end());

        std::ranges::for_each(boundary_edges, [&triangulation, &point](const Edge &edge) {
            triangulation.emplace_back(edge.p1, edge.p2, point);
        });
    };

    std::ranges::for_each(points, watson_bowyer_algo);

    auto it = std::ranges::remove_if(triangulation, [&super1, &super2, &super3](const DelaunayTriangle &tri) {
        auto vertices = tri.vertices();
        return std::ranges::any_of(vertices, [&super1, &super2, &super3](const Point2D &vertex) {
            return vertex == super1 || vertex == super2 || vertex == super3;
        });
    });

    triangulation.erase(it.begin(), it.end());

    return triangulation;
}
}  // namespace geometry::triangulation

template <>
struct std::formatter<geometry::triangulation::DelaunayTriangle> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::triangulation::DelaunayTriangle &t, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "DelaunayTriangle({}, {}, {})", t.a, t.b, t.c);
    }
};
