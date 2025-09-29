#include "convex_hull.hpp"
#include "geometry.hpp"
#include "intersections.hpp"
#include "queries.hpp"
#include "shape_utils.hpp"
#include "triangulation.hpp"
#include "visualization.hpp"

#include <algorithm>
#include <print>
#include <ranges>
#include <variant>
#include <vector>

using namespace geometry;

namespace rng = std::ranges;
namespace views = std::ranges::views;

static inline std::string shape_to_string(const Shape &shape) {
    return std::visit([](const auto &cs) { return std::format("{}", cs); }, shape);
}

static inline bool is_support_intersection(const Shape &shape) {
    return std::holds_alternative<Line>(shape) || std::holds_alternative<Circle>(shape);
};

void PrintAllIntersections(const Shape &shape, std::vector<Shape> others) {
    std::println("\n=== Intersections ===");

    if (!is_support_intersection(shape)) {
        std::println("Shape {} not support intersactions.", shape_to_string(shape));
        return;
    }

    auto supported_shapes = views::filter(others, is_support_intersection) | rng::to<std::vector>();
    auto shapes_pairs = views::cartesian_product(std::vector<Shape>{shape}, supported_shapes);
    rng::for_each(shapes_pairs, [](const std::tuple<Shape, Shape> &shapes) {
        Shape shape1 = std::get<0>(shapes);
        Shape shape2 = std::get<1>(shapes);
        auto res = geometry::intersections::GetIntersectPoint(shape1, shape2);

        if (res)
            std::println("Intersection found in points [{}] between shapes {} and {}", *res, shape_to_string(shape1),
                         shape_to_string(shape2));
        else
            std::println("Shapes {} and {} doesn't intersect", shape_to_string(shape1), shape_to_string(shape2));
    });
}

void PrintDistancesFromPointToShapes(Point2D p, std::vector<Shape> shapes) {
    std::println("\n=== Distance from Point Test ===");
    std::println("Testing point: {} ", p);

    std::vector<Shape> out;
    auto gen = std::mt19937{std::random_device{}()};
    rng::sample(shapes, std::back_inserter(out), 5, gen);
    rng::for_each(out, [&p](const Shape &shape) {
        auto res = geometry::queries::DistanceToPoint(shape, p);

        if (res)
            std::println("Distance from point {} to shape {} is {}", p, shape_to_string(shape), *res);
        else
            std::println("Distance from point {} to shape {} can't be found.", p, shape_to_string(shape));
    });
}

void PerformShapeAnalysis(std::vector<Shape> shapes) {
    std::println("\n=== Shape Analysis ===");

    auto max_hight = geometry::utils::FindHighestShape(shapes);
    if (max_hight.has_value())
        std::println("Shape with max height : {}, height = {}", shape_to_string(max_hight.value()),
                     geometry::queries::GetHeight(max_hight.value()));
    else
        std::println("Can't find highest shape");

    auto supported_shapes = views::filter(shapes, is_support_intersection) | rng::to<std::vector>();

    for (size_t i = 0; i < supported_shapes.size() - 1; i++) {
        Shape shape1 = supported_shapes[i];
        for (size_t j = i + 1; j < supported_shapes.size(); j++) {
            Shape shape2 = supported_shapes[j];
            auto res = geometry::intersections::GetIntersectPoint(shape1, shape2);

            if (res)
                std::println("Intersection found in points [{}] between shapes {} and {}", *res,
                             shape_to_string(shape1), shape_to_string(shape2));
            else
                std::println("Shapes {} and {} doesn't intersect", shape_to_string(shape1), shape_to_string(shape2));
        }
    }

    std::vector<Shape> out;
    auto gen = std::mt19937{std::random_device{}()};
    rng::sample(supported_shapes, std::back_inserter(out), 2, gen);
    if (out.size() < 2)
        std::println("There is not enough distanse supported shapes.");
    else {
        Shape shape1 = out[0];
        Shape shape2 = out[1];
        auto res = geometry::queries::DistanceBetweenShapes(shape1, shape2);
        if (res)
            std::println("Distance between shapes {} and {} is {}", shape_to_string(shape1), shape_to_string(shape2),
                         *res);
        else
            std::println("Can't measure distance between {} and {}", shape_to_string(shape1), shape_to_string(shape2));
    }
}

void PerformExtraShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Extra Analysis ===");

    auto shapes_above_50 =
        views::filter(shapes, [](const Shape &shape) { return geometry::queries::GetHeight(shape) > 50.0; }) |
        rng::to<std::vector>();

    std::vector<Shape> out;
    auto gen = std::mt19937{std::random_device{}()};
    rng::sample(shapes_above_50, std::back_inserter(out), std::min(3ul, shapes_above_50.size()), gen);

    auto min_hight = *rng::min_element(shapes, [](const Shape &shape1, const Shape &shape2) {
        return geometry::queries::GetHeight(shape1) < geometry::queries::GetHeight(shape2);
    });
    auto max_hight = *rng::max_element(shapes, [](const Shape &shape1, const Shape &shape2) {
        return geometry::queries::GetHeight(shape1) < geometry::queries::GetHeight(shape2);
    });

    std::println("{} shapes with hight above 50", out.size());
    for (auto shape : out) {
        std::println("{}, height = {}", shape_to_string(shape), geometry::queries::GetHeight(shape));
    }

    std::println("Shape with max height : {}, height = {}", shape_to_string(max_hight),
                 geometry::queries::GetHeight(max_hight));

    std::println("Shape with min height : {}, height = {}", shape_to_string(min_hight),
                 geometry::queries::GetHeight(min_hight));
}

int main() {
    utils::ShapeGenerator generator(-50.0, 50.0, 5.0, 25.0);
    const size_t shapes_cnt = 25;
    std::vector<Shape> shapes = generator.GenerateShapes(shapes_cnt);

    std::println("Generated {} random shapes", shapes.size());

    // Выведите индекс каждой фигуры и её высоту
    std::println("=== Heights of shapes ===");
    for (size_t index = 0; index < shapes_cnt; index++) {
        std::println("[{}] shape height {}", index, queries::GetHeight(shapes[index]));
    }

    //
    // Вызываем разработанные функции
    //
    PrintAllIntersections(shapes[0], shapes);

    PrintDistancesFromPointToShapes(Point2D{10.0, 10.0}, shapes);

    PerformShapeAnalysis(shapes);

    PerformExtraShapeAnalysis(shapes);

    //
    // Рисуем все фигуры
    //
    // Важно: после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 2ой график
    //
    geometry::visualization::Draw(shapes);

    //
    // Формируем список из вершин всех фигур
    //
    std::vector<Point2D> points = convex_hull::GrahamScan(shapes).value_or(std::vector<Point2D>{});
    std::println("Number of points {}", points.size());
    //
    // Находим список точек, для построения выпуклой оболочки - convex hull - алгоритмом Грэхема
    // Создаём из них объект класса `Polygon` и добавляем его в список shapes
    // Рисуем все фигуры
    //
    shapes.emplace_back(geometry::Polygon{points});
    geometry::visualization::Draw(shapes);

    //
    // после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 3ий график
    //

    {
        std::vector<Point2D> points = shapes | std::ranges::views::transform([](const auto &shape) {
                                          return geometry::queries::GetVertexes(shape).value_or(std::vector<Point2D>{});
                                      }) |
                                      std::ranges::views::join |
                                      std::ranges::to<std::vector>();  //{{0, 0}, {10, 0}, {5, 8}, {15, 5}, {2, 12}};

        //
        // Используйте список точек points или свой, чтобы
        // выполнить алгоритм триангуляции Делоне алгоритмом Боуэра-Ватсона
        //
        // После успешного завершения алгоритма - выведите результат для проверки
        // используя geometry::visualization::Draw
        //
        auto res = geometry::triangulation::DelaunayTriangulation(points);
        if (res) {
            std::vector<geometry::triangulation::DelaunayTriangle> triangles = res.value();
            geometry::visualization::Draw(triangles);
        } else {
            std::println("Triangulation failed: {}", static_cast<int>(res.error()));
        }
    }
    return 0;
}