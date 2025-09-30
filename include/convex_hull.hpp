#pragma once
#include "geometry.hpp"
#include <algorithm>
#include <ranges>
#include <stack>
#include <vector>

namespace geometry::convex_hull {

double CrossProduct(const Point2D &p1, const Point2D &middle, const Point2D &p2);

class StackForGrahamScan {
public:
    void Push(const Point2D &p) { s.push_back(p); }
    void Pop() { s.pop_back(); }

    size_t Size() { return s.size(); }
    Point2D Top() { return s.back(); }
    Point2D NextToTop() { return *std::prev(s.end(), 2); }

    std::vector<Point2D> &&Extract() && { return std::move(s); }

private:
    std::vector<Point2D> s;
};

GeometryResult<std::vector<Point2D>> GrahamScan(std::vector<Shape> points);

}  // namespace geometry::convex_hull