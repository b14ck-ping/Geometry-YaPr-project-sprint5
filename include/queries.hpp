#pragma once
#include "geometry.hpp"
#include <algorithm>
#include <optional>
#include <stdexcept>
#include <variant>

namespace geometry::queries {

template <class... Ts>
struct Multilambda : Ts... {
    using Ts::operator()...;
};

/*
 * Класс для поиска расстояния от точки до фигуры
 *
 * Требуется организовать возможность нахождения расстояния для всех возможных фигур типа-суммы Shape
 */
struct PointToShapeDistanceVisitor {
    Point2D point;

    explicit PointToShapeDistanceVisitor(const Point2D &p) : point(p) {}

    /* ваш код здесь */
    std::optional<double> operator()(const Line &line) const {
        if (line.start.DistanceTo(line.end) < 1e-10) {
            return point.DistanceTo(line.start);
        }

        Point2D AB = line.end - line.start;
        Point2D AP = point - line.start;

        double ab2 = AB.Dot(AB);
        double t = AP.Dot(AB) / ab2;
        t = std::max(0.0, std::min(1.0, t));

        Point2D closest = line.start + AB * t;
        return point.DistanceTo(closest);
    }

    std::optional<double> operator()(const Triangle &tr) const {
        Point2D v0 = tr.c - tr.a;
        Point2D v1 = tr.b - tr.a;
        Point2D v2 = point - tr.a;

        double dot00 = v0.Dot(v0);
        double dot01 = v0.Dot(v1);
        double dot02 = v0.Dot(v2);
        double dot11 = v1.Dot(v1);
        double dot12 = v1.Dot(v2);

        double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
        double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

        if ((u >= 0) && (v >= 0) && (u + v <= 1)) {
            return 0.0;
        }

        double distAB = PointToShapeDistanceVisitor{point}(Line{tr.a, tr.b}).value_or(0.0);
        double distBC = PointToShapeDistanceVisitor{point}(Line{tr.b, tr.c}).value_or(0.0);
        double distCA = PointToShapeDistanceVisitor{point}(Line{tr.c, tr.a}).value_or(0.0);

        return std::min({distAB, distBC, distCA});
    }

    std::optional<double> operator()(const Rectangle &rect) const {
        if (point.x >= rect.bottom_left.x && point.x <= rect.bottom_left.x + rect.width &&
            point.y >= rect.bottom_left.y && point.y <= rect.bottom_left.y + rect.height) {
            return 0.0;
        }

        auto vertices = rect.Vertices();

        double dist1 = PointToShapeDistanceVisitor{point}(Line{vertices[0], vertices[1]}).value_or(0.0);
        double dist2 = PointToShapeDistanceVisitor{point}(Line{vertices[1], vertices[2]}).value_or(0.0);
        double dist3 = PointToShapeDistanceVisitor{point}(Line{vertices[2], vertices[3]}).value_or(0.0);
        double dist4 = PointToShapeDistanceVisitor{point}(Line{vertices[3], vertices[0]}).value_or(0.0);

        return std::min({dist1, dist2, dist3, dist4});
    }

    std::optional<double> operator()(const RegularPolygon &poly) const {
        if (poly.radius < 1e-10) {
            return point.DistanceTo(poly.center_p);
        }

        if (poly.sides < 3) {
            return point.DistanceTo(poly.center_p);
        }

        auto vertices = poly.Vertices();
        int n = poly.sides;

        double dist_to_center = point.DistanceTo(poly.center_p);

        if (dist_to_center <= poly.radius) {
            bool inside = false;
            for (int i = 0, j = n - 1; i < n; j = i++) {
                if (((vertices[i].y > point.y) != (vertices[j].y > point.y)) &&
                    (point.x <
                     (vertices[j].x - vertices[i].x) * (point.y - vertices[i].y) / (vertices[j].y - vertices[i].y) +
                         vertices[i].x)) {
                    inside = !inside;
                }
            }

            if (inside) {
                return 0.0;
            }
        }

        for (int i = 0; i < n; ++i) {
            Point2D a = vertices[i];
            Point2D b = vertices[(i + 1) % n];

            Point2D ab = b - a;
            Point2D ap = point - a;

            double ab_length_sq = ab.Dot(ab);
            double dot = ap.Dot(ab);
            double t = dot / ab_length_sq;

            if (t >= 0.0 && t <= 1.0) {
                Point2D projection = a + ab * t;
                if (point.DistanceTo(projection) < 1e-10) {
                    return 0.0;
                }
            }
        }

        double min_distance = std::numeric_limits<double>::max();
        for (int i = 0; i < n; ++i) {
            double dist =
                PointToShapeDistanceVisitor{point}(Line{vertices[i], vertices[(i + 1) % n]}).value_or(min_distance);
            if (dist < min_distance) {
                min_distance = dist;
            }
        }

        return min_distance;
    }

    std::optional<double> operator()(const Circle &circle) const {
        double distance_to_center = point.DistanceTo(circle.Center());
        if (circle.radius)
            return std::abs(distance_to_center - circle.radius);
        else
            return distance_to_center;
    }

    std::optional<double> operator()(const Polygon &poly) const {
        auto vertices = poly.Vertices();
        int n = vertices.size();

        if (n == 0)
            return std::nullopt;
        if (n == 1)
            return point.DistanceTo(vertices[0]);
        if (n == 2)
            return PointToShapeDistanceVisitor{point}(Line{vertices[0], vertices[1]});

        for (int i = 0; i < n; ++i) {
            Point2D a = vertices[i];
            Point2D b = vertices[(i + 1) % n];

            Point2D ab = b - a;
            Point2D ap = point - a;

            double ab_length_sq = ab.Dot(ab);
            double dot = ap.Dot(ab);
            double t = dot / ab_length_sq;

            if (t >= 0.0 && t <= 1.0) {
                Point2D projection = a + ab * t;
                if (point.DistanceTo(projection) < 1e-10) {
                    return 0.0;
                }
            }
        }

        bool inside = false;
        for (int i = 0, j = n - 1; i < n; j = i++) {
            if (((vertices[i].y > point.y) != (vertices[j].y > point.y)) &&
                (point.x <
                 (vertices[j].x - vertices[i].x) * (point.y - vertices[i].y) / (vertices[j].y - vertices[i].y) +
                     vertices[i].x)) {
                inside = !inside;
            }
        }

        if (inside)
            return 0.0;

        double min_distance = std::numeric_limits<double>::max();
        for (int i = 0; i < n; ++i) {
            double dist =
                PointToShapeDistanceVisitor{point}(Line{vertices[i], vertices[(i + 1) % n]}).value_or(min_distance);
            if (dist < min_distance)
                min_distance = dist;
        }

        return min_distance;
    }

    std::optional<double> operator()(const auto &) const { return std::nullopt; }
};

/*
 * Класс для поиска расстояния между двумя фигурами
 *
 * Требуется организовать возможность нахождения расстояния только для следующих комбинаций фигур:
 *    - Line   & Line
 *    - Circle & Circle
 *
 * Важно: вы можете выбрать любой метод нахождения расстояния, даже если он даёт не точный результат
 *
 * Для всех остальных требуется вернуть пустое значение
 */
struct ShapeToShapeDistanceVisitor {

    /* ваш код здесь */
    std::optional<double> operator()(const Circle &circle1, const Circle &circle2) const {
        double distance_between_centers = circle1.center_p.DistanceTo(circle2.center_p);
        return std::max(0.0, distance_between_centers - circle1.radius - circle2.radius);
    }

    std::optional<double> operator()(const Line &line1, const Line &line2) const {
        if (areLinesCollinearAndOverlapping(line1, line2)) {
            return 0.0;
        }

        double d1 = line1.start.DistanceTo(line2.start);
        double d2 = line1.start.DistanceTo(line2.end);
        double d3 = line1.end.DistanceTo(line2.start);
        double d4 = line1.end.DistanceTo(line2.end);

        return std::min({d1, d2, d3, d4});
    }

    std::optional<double> operator()(const auto &shape1, const auto &shape2) const { return std::nullopt; }

private:
    bool areLinesCollinearAndOverlapping(const Line &line1, const Line &line2) const {
        Point2D dir1 = line1.end - line1.start;
        Point2D dir2 = line2.end - line2.start;

        if (std::abs(dir1.Cross(dir2)) > 1e-10) {
            return false;
        }

        auto bbox1 = line1.BoundBox();
        auto bbox2 = line2.BoundBox();

        return (bbox1.min_x <= bbox2.max_x && bbox1.max_x >= bbox2.min_x && bbox1.min_y <= bbox2.max_y &&
                bbox1.max_y >= bbox2.min_y);
    }
};

/*
 * Функции-помощники
 */
inline std::optional<double> DistanceToPoint(const Shape &shape, const Point2D &point) {
    return std::visit(PointToShapeDistanceVisitor{point}, shape);
}

inline BoundingBox GetBoundBox(const Shape &shape) {

    return std::visit(
        Multilambda{[](const Line &line) { return line.BoundBox(); }, [](const Triangle &tr) { return tr.BoundBox(); },
                    [](const Rectangle &rect) { return rect.BoundBox(); },
                    [](const RegularPolygon &regpol) { return regpol.BoundBox(); },
                    [](const Polygon &polygon) { return polygon.BoundBox(); },
                    [](const Circle &circle) { return circle.BoundBox(); },
                    [](const auto &) -> BoundingBox { throw std::logic_error("Unsupported type in GetBoundBox"); }},
        shape);
}

inline double GetHeight(const Shape &shape) {

    return std::visit(
        Multilambda{[](const Line &line) { return line.Height(); }, [](const Triangle &tr) { return tr.Height(); },
                    [](const Rectangle &rect) { return rect.Height(); },
                    [](const RegularPolygon &regpol) { return regpol.Height(); },
                    [](const Polygon &polygon) { return polygon.Height(); },
                    [](const Circle &circle) { return circle.Height(); },
                    [](const auto &) -> BoundingBox { throw std::logic_error("Unsupported type in GetBoundBox"); }},
        shape);
}

inline bool BoundingBoxesOverlap(const Shape &shape1, const Shape &shape2) {
    BoundingBox box1 = GetBoundBox(shape1);
    BoundingBox box2 = GetBoundBox(shape2);

    return box1.Overlaps(box2);
}

inline std::optional<double> DistanceBetweenShapes(const Shape &shape1, const Shape &shape2) {
    return std::visit(ShapeToShapeDistanceVisitor{}, shape1, shape2);
}

inline std::optional<std::vector<Point2D>> GetVertexes(const Shape &shape) {

    return std::visit(Multilambda{[](const Line &line) -> std::vector<Point2D> {
                                      auto vert = line.Vertices();
                                      return std::vector(std::begin(vert), std::end(vert));
                                  },
                                  [](const Triangle &tr) -> std::vector<Point2D> {
                                      auto vert = tr.Vertices();
                                      return std::vector(std::begin(vert), std::end(vert));
                                  },
                                  [](const Rectangle &rect) -> std::vector<Point2D> {
                                      auto vert = rect.Vertices();
                                      return std::vector(std::begin(vert), std::end(vert));
                                  },
                                  [](const RegularPolygon &regpol) -> std::vector<Point2D> {
                                      auto vert = regpol.Vertices();
                                      return std::vector(std::begin(vert), std::end(vert));
                                  },
                                  [](const Polygon &polygon) -> std::vector<Point2D> {
                                      auto vert = polygon.Vertices();
                                      return std::vector(std::begin(vert), std::end(vert));
                                  },
                                  [](const Circle &circle) -> std::vector<Point2D> {
                                      auto vert = circle.Vertices();
                                      return std::vector(std::begin(vert), std::end(vert));
                                  },
                                  [](const auto &) -> std::vector<Point2D> {
                                      throw std::logic_error("Unsupported type in GetBoundBox");
                                  }},
                      shape);
}

}  // namespace geometry::queries