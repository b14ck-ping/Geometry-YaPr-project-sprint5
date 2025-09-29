#pragma once
#include "geometry.hpp"
#include <cmath>
#include <optional>
#include <print>
#include <utility>

namespace geometry::intersections {

static inline bool s_check_point_on_the_line(Point2D point, const Line &line) {
    double max_x = line.start.x;
    double min_x = line.end.x;
    double max_y = line.start.y;
    double min_y = line.end.y;

    if (max_x < min_x)
        std::swap(max_x, min_x);
    if (max_y < min_y)
        std::swap(max_y, min_y);

    if ((point.x <= max_x && point.x >= min_x) && (point.y <= max_y && point.y >= min_y))
        return true;
    else
        return false;
    ;
}
/*
 * Класс для поиска пересечений между двумя фигурами
 *
 * Требуется организовать возможность нахождения пересечений только для следующих комбинаций фигур:
 *    - Line   & Line
 *    - Circle & Circle
 *
 * Для всех остальных требуется вернуть std::nullopt
 */
class IntersectionVisitor {
public:
    std::optional<std::vector<Point2D>> operator()(const Line &line1, const Line &line2) const {
        double a_1 = (line1.end.y - line1.start.y) / (line1.end.x - line1.start.x);
        double b_1 = line1.start.y - a_1 * line1.start.x;

        double a_2 = (line2.end.y - line2.start.y) / (line2.end.x - line2.start.x);
        double b_2 = line2.start.y - a_2 * line2.start.x;

        if (a_1 == a_2)
            return std::nullopt;

        double intersection_x = (b_2 - b_1) / (a_1 - a_2);
        double intersection_y = a_1 * intersection_x + b_1;

        Point2D int_point = {intersection_x, intersection_y};

        if (s_check_point_on_the_line(int_point, line1) && s_check_point_on_the_line(int_point, line2))
            return std::vector{int_point};
        else
            return std::nullopt;
    }

    std::optional<std::vector<Point2D>> operator()(const Circle &circle1, const Circle &circle2) const {
        std::vector<Point2D> intersections;

        double dx = circle2.center_p.x - circle1.center_p.x;
        double dy = circle2.center_p.y - circle1.center_p.y;

        double d = std::sqrt(dx * dx + dy * dy);

        if (d > circle1.radius + circle2.radius) {
            return std::nullopt;
        }

        if (d < std::abs(circle1.radius - circle2.radius)) {
            return std::nullopt;
        }

        if (d < 1e-10 && std::abs(circle1.radius - circle2.radius) < 1e-10) {
            return std::nullopt;
        }

        double a = (circle1.radius * circle1.radius - circle2.radius * circle2.radius + d * d) / (2 * d);
        double h = std::sqrt(circle1.radius * circle1.radius - a * a);

        Point2D p2;
        p2.x = circle1.center_p.x + (a * dx) / d;
        p2.y = circle1.center_p.y + (a * dy) / d;

        if (std::abs(h) < 1e-10) {
            intersections.push_back(p2);
            return intersections;
        }

        double offsetX = (-dy * h) / d;
        double offsetY = (dx * h) / d;

        Point2D intersection1, intersection2;
        intersection1.x = p2.x + offsetX;
        intersection1.y = p2.y + offsetY;
        intersection2.x = p2.x - offsetX;
        intersection2.y = p2.y - offsetY;

        intersections.push_back(intersection1);
        intersections.push_back(intersection2);

        return intersections;
    }

    std::optional<std::vector<Point2D>> operator()(const Line &line, const Circle &circle) const {
        std::vector<Point2D> result;

        if (circle.radius < 1e-10) {
            Point2D circle_center = circle.center_p;

            Point2D v1 = {circle_center.x - line.start.x, circle_center.y - line.start.y};
            Point2D v2 = {line.end.x - line.start.x, line.end.y - line.start.y};

            double cross = v1.Cross(v2);
            double dot = v1.Dot(v2);
            double length_sq = v2.x * v2.x + v2.y * v2.y;

            if (std::abs(cross) < 1e-10 && dot >= 0 && dot <= length_sq) {
                result.push_back(circle_center);
                return result;
            }
            return std::nullopt;
        }

        if (std::abs(line.start.x - line.end.x) < 1e-10 && std::abs(line.start.y - line.end.y) < 1e-10) {
            double dx = line.start.x - circle.center_p.x;
            double dy = line.start.y - circle.center_p.y;
            double distance_sq = dx * dx + dy * dy;

            if (std::abs(distance_sq - circle.radius * circle.radius) < 1e-10) {
                result.push_back(line.start);
                return result;
            }
            return std::nullopt;
        }

        double x1 = line.start.x - circle.center_p.x;
        double y1 = line.start.y - circle.center_p.y;
        double x2 = line.end.x - circle.center_p.x;
        double y2 = line.end.y - circle.center_p.y;

        double dx = x2 - x1;
        double dy = y2 - y1;

        double a = dx * dx + dy * dy;
        double b = 2 * (x1 * dx + y1 * dy);
        double c = x1 * x1 + y1 * y1 - circle.radius * circle.radius;

        double discriminant = b * b - 4 * a * c;

        if (discriminant < 0 || a == 0) {
            return std::nullopt;
        }

        double t1 = (-b - std::sqrt(discriminant)) / (2 * a);
        double t2 = (-b + std::sqrt(discriminant)) / (2 * a);

        if (t1 >= 0 && t1 <= 1) {
            Point2D intersection;
            intersection.x = line.start.x + t1 * dx;
            intersection.y = line.start.y + t1 * dy;
            result.push_back(intersection);
        }

        if (discriminant > 0 && t2 >= 0 && t2 <= 1) {
            Point2D intersection;
            intersection.x = line.start.x + t2 * dx;
            intersection.y = line.start.y + t2 * dy;
            result.push_back(intersection);
        }

        if (result.empty())
            return std::nullopt;

        return result;
    }

    std::optional<std::vector<Point2D>> operator()(const Circle &circle, const Line &line) const {
        return IntersectionVisitor{}(line, circle);
    }

    std::optional<std::vector<Point2D>> operator()(const auto &a, const auto &b) const { return std::nullopt; }
};

inline std::optional<std::vector<Point2D>> GetIntersectPoint(const Shape &shape1, const Shape &shape2) {
    return std::visit(IntersectionVisitor{}, shape1, shape2);
}

}  // namespace geometry::intersections