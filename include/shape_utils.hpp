#pragma once
#include "geometry.hpp"
#include "queries.hpp"
#include <print>
#include <random>
#include <ranges>
#include <utility>
#include <vector>

namespace geometry::utils {

class ShapeGenerator {
public:
    ShapeGenerator(double min_coord = -100.0, double max_coord = 100.0, double min_size = 1.0, double max_size = 20.0)
        : gen(20), coord_dist(min_coord, max_coord), size_dist(min_size, max_size), sides_dist(3, 12), type_dist(0, 4) {
    }

    Shape GenerateRandomShape() {
        Point2D center{coord_dist(gen), coord_dist(gen)};
        double size = size_dist(gen);

        switch (type_dist(gen)) {
        case 0: {
            Point2D end{center.x + size, center.y + size};
            return Line{center, end};
        }
        case 1: {
            Point2D a{center.x, center.y};
            Point2D b{center.x + size, center.y};
            Point2D c{center.x + size / 2, center.y + size};
            return Triangle{a, b, c};
        }
        case 2: {
            return Rectangle{center, size, size * 0.8};
        }
        case 3: {
            int sides = sides_dist(gen);
            return RegularPolygon{center, size, sides};
        }
        case 4: {
            return Circle{center, size};
        }
        }
        return Circle{center, size};
    }

    std::vector<Shape> GenerateShapes(size_t count) {
        std::vector<Shape> shapes;
        shapes.reserve(count);

        for (auto _ : std::views::iota(0u, count)) {
            shapes.push_back(GenerateRandomShape());
        }

        return shapes;
    }

private:
    std::mt19937 gen;
    std::uniform_real_distribution<double> coord_dist;
    std::uniform_real_distribution<double> size_dist;
    std::uniform_int_distribution<int> sides_dist;
    std::uniform_int_distribution<int> type_dist;
};

inline std::vector<std::pair<Shape, Shape>> FindAllCollisions(const std::vector<Shape> shapes) {
    std::vector<std::pair<Shape, Shape>> collisions;

    /*
     * Используйте библиотеку ranges, чтобы найти все коллизии между фигурами методом BoundingBoxesOverlap
     *
     * Также используйте наиболее эффективный метод добавления объектов в collisions
     */

    size_t n = shapes.size();
    auto make_index_pair = [&n](size_t i) {
        return std::views::iota(i + 1, n)  // Indexes j от i+1 до n-1
               | std::views::transform([i](size_t j) { return std::make_pair(i, j); });
    };

    auto check_shapes_pair =
        [&shapes](const std::pair<size_t, size_t> &index_pair) -> std::optional<std::pair<Shape, Shape>> {
        Shape shape1 = shapes.at(index_pair.first);
        Shape shape2 = shapes.at(index_pair.second);

        if (geometry::queries::BoundingBoxesOverlap(shape1, shape2))
            return std::pair<Shape, Shape>{shape1, shape2};
        else
            return std::nullopt;
    };

    return std::views::iota(0uz, n) | std::views::transform(make_index_pair) | std::views::join |
           std::views::transform(check_shapes_pair) |
           std::views::filter([](const auto &opt) { return opt.has_value(); }) |
           std::views::transform([](const auto &opt) { return *opt; }) | std::ranges::to<std::vector>();
}

inline std::optional<Shape> FindHighestShape(const std::vector<Shape> shapes) {

    /*
     * Используйте библиотеку ranges, чтобы найти самую высокую фигуру
     *
     * Важно: использование ручной итерации по фигурам не разрешается
     */
    if (shapes.empty())
        return std::nullopt;

    Shape highest_shape = std::ranges::max(shapes, [](const auto &shape1, const auto &shape2) {
        return geometry::queries::GetHeight(shape1) < geometry::queries::GetHeight(shape2);
    });

    return highest_shape;
}

}  // namespace geometry::utils