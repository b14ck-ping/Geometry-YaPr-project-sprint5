#include "convex_hull.hpp"
#include "geometry.hpp"
#include "queries.hpp"
#include <algorithm>
#include <expected>
#include <iostream>
#include <ranges>

namespace geometry::convex_hull {

double CrossProduct(const Point2D &p1, const Point2D &middle, const Point2D &p2) {
    auto new_p1 = p1 - middle;
    auto new_p2 = p2 - middle;
    return new_p1.Cross(new_p2);
}

// let points be the list of points
// let stack = empty_stack()

// find the lowest y-coordinate and leftmost point, called P0
// sort points by polar angle with P0, if several points have the same polar angle then only keep the farthest

// for point in points:
//     # pop the last point from the stack if we turn clockwise to reach this point
//     while count stack > 1 and ccw(next_to_top(stack), top(stack), point) <= 0:
//         pop stack
//     push point to stack
// end

static inline std::vector<geometry::Point2D> SortByPolarAngle(std::vector<geometry::Point2D> points,
                                                              const geometry::Point2D &P0) {
    if (points.empty())
        return points;

    auto it_remove = std::remove_if(points.begin(), points.end(), [&P0](const Point2D &p) {
        return CheckDoubleIsZeroWithTolerance(p.x - P0.x) && CheckDoubleIsZeroWithTolerance(p.y - P0.y);
    });
    points.erase(it_remove, points.end());

    if (points.empty()) {
        points.push_back(P0);
        return points;
    }

    auto angle_comp = [&P0](const geometry::Point2D &a, const geometry::Point2D &b) {
        geometry::Point2D vecA = a - P0;
        geometry::Point2D vecB = b - P0;

        double cr = CrossProduct(P0, a, b);
        if (!CheckDoubleIsZeroWithTolerance(cr))
            return cr > 0;

        return vecA.Length() > vecB.Length();
    };

    std::ranges::sort(points, angle_comp);

    auto unique_it = std::unique(points.begin(), points.end(), [&P0](const Point2D &a, const Point2D &b) {
        geometry::Point2D vecA = a - P0;
        geometry::Point2D vecB = b - P0;
        double angleA = std::atan2(vecA.y, vecA.x);
        double angleB = std::atan2(vecB.y, vecB.x);
        return CheckDoubleIsZeroWithTolerance(angleA - angleB);
    });
    points.erase(unique_it, points.end());

    points.insert(points.begin(), P0);

    return points;
}

GeometryResult<std::vector<Point2D>> GrahamScan(std::vector<Shape> shapes) {
    namespace rv = std::ranges::views;
    namespace rs = std::ranges;

    if (shapes.empty())
        return std::unexpected{GeometryError::InvalidInput};

    StackForGrahamScan stack{};

    std::vector<Point2D> points = shapes | rv::transform([](const auto &shape) -> std::vector<Point2D> {
                                      return geometry::queries::GetVertexes(shape).value_or(std::vector<Point2D>{});
                                  }) |
                                  rv::join | rs::to<std::vector>();

    if (points.size() < 3)
        return std::unexpected{GeometryError::InsufficientPoints};

    // find the lowest y-coordinate and leftmost point, called P0
    auto find_p0 = [](const Point2D &a, const Point2D &b) {
        if (a.y < b.y)
            return true;
        if (a.y > b.y)
            return false;
        return a.x < b.x;
    };
    Point2D P0 = *rs::min_element(points, find_p0);

    auto graham_scan = [&stack](const Point2D &point) {
        // pop the last point from the stack if we turn clockwise to reach this point
        while (stack.Size() > 1) {
            Point2D top_point = stack.Top();
            Point2D next_to_top = stack.NextToTop();
            double cross = CrossProduct(next_to_top, top_point, point);
            // pop while cross <= 0: non-left turn (clockwise or collinear towards interior)
            if (cross <= 0) {
                stack.Pop();
            } else {
                break;
            }
        }
        stack.Push(point);
    };

    // sort points by polar angle with P0, if several points
    // have the same polar angle then only keep the farthest
    points = SortByPolarAngle(points, P0);
    if (points.size() < 3)
        return std::unexpected{GeometryError::InsufficientPoints};

    stack.Push(points[0]);
    stack.Push(points[1]);

    rs::for_each(rs::begin(points) + 2, rs::end(points), graham_scan);

    return std::move(stack).Extract();
}

}  // namespace geometry::convex_hull