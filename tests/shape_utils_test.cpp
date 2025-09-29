#include "geometry.hpp"
#include <../include/shape_utils.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <print>

using namespace geometry;
using namespace geometry::utils;
using namespace geometry::queries;

class TestShapeCreator {
public:
    static geometry::Line CreateLine(double x1, double y1, double x2, double y2) {
        return geometry::Line{{x1, y1}, {x2, y2}};
    }

    static geometry::Rectangle CreateRectangle(double x, double y, double width, double height) {
        return geometry::Rectangle{{x, y}, width, height};
    }

    static geometry::Circle CreateCircle(double x, double y, double radius) { return geometry::Circle{{x, y}, radius}; }

    static geometry::Triangle CreateTriangle(double x1, double y1, double x2, double y2, double x3, double y3) {
        return geometry::Triangle{{x1, y1}, {x2, y2}, {x3, y3}};
    }

    static geometry::RegularPolygon CreateRegularPolygon(double x, double y, double radius, int sides) {
        return geometry::RegularPolygon{{x, y}, radius, sides};
    }

    static geometry::Polygon CreatePolygon(const std::vector<geometry::Point2D> &points) {
        return geometry::Polygon(points);
    }
};

class FindAllCollisionsTest : public ::testing::Test {
protected:
    TestShapeCreator creator;
};

TEST_F(FindAllCollisionsTest, EmptyInputReturnsEmptyCollisions) {
    std::vector<geometry::Shape> shapes;
    auto collisions = FindAllCollisions(std::move(shapes));
    EXPECT_TRUE(collisions.empty());
}

TEST_F(FindAllCollisionsTest, SingleShapeReturnsEmptyCollisions) {
    std::vector<geometry::Shape> shapes = {creator.CreateRectangle(0, 0, 1, 1)};
    auto collisions = FindAllCollisions(std::move(shapes));
    EXPECT_TRUE(collisions.empty());
}

TEST_F(FindAllCollisionsTest, TwoOverlappingRectanglesReturnOneCollision) {
    auto rect1 = creator.CreateRectangle(0, 0, 2, 2);
    auto rect2 = creator.CreateRectangle(1, 1, 2, 2);

    std::vector<geometry::Shape> shapes = {rect1, rect2};
    auto collisions = FindAllCollisions(std::move(shapes));

    EXPECT_EQ(collisions.size(), 1);
    EXPECT_TRUE(BoundingBoxesOverlap(collisions[0].first, collisions[0].second));
}

TEST_F(FindAllCollisionsTest, TwoNonOverlappingShapesReturnEmpty) {
    std::vector<geometry::Shape> shapes = {creator.CreateRectangle(0, 0, 1, 1), creator.CreateRectangle(3, 3, 1, 1)};

    auto collisions = FindAllCollisions(std::move(shapes));
    EXPECT_TRUE(collisions.empty());
}

TEST_F(FindAllCollisionsTest, MixedShapeTypesWithOverlap) {
    std::vector<geometry::Shape> shapes = {creator.CreateLine(0, 0, 3, 3), creator.CreateCircle(1, 1, 1.5),
                                           creator.CreateRectangle(2, 2, 2, 2)};

    auto collisions = FindAllCollisions(std::move(shapes));

    EXPECT_EQ(collisions.size(), 3);

    for (const auto &collision : collisions) {
        EXPECT_TRUE(BoundingBoxesOverlap(collision.first, collision.second));
    }
}

TEST_F(FindAllCollisionsTest, MultipleShapesWithPartialOverlap) {
    auto rect1 = creator.CreateRectangle(0, 0, 2, 2);
    auto rect2 = creator.CreateRectangle(1, 1, 2, 2);
    auto rect3 = creator.CreateRectangle(4, 4, 1, 1);
    auto circle = creator.CreateCircle(0.5, 0.5, 1);

    std::vector<geometry::Shape> shapes = {rect1, rect2, rect3, circle};
    auto collisions = FindAllCollisions(std::move(shapes));

    EXPECT_EQ(collisions.size(), 3);

    for (const auto &collision : collisions) {
        EXPECT_TRUE(BoundingBoxesOverlap(collision.first, collision.second));
    }
}

TEST_F(FindAllCollisionsTest, TouchingShapesAreConsideredOverlapping) {
    std::vector<geometry::Shape> shapes = {creator.CreateRectangle(0, 0, 1, 1), creator.CreateRectangle(1, 0, 1, 1),
                                           creator.CreateRectangle(0, 1, 1, 1)};

    auto collisions = FindAllCollisions(std::move(shapes));
    EXPECT_EQ(collisions.size(), 3);
}

TEST_F(FindAllCollisionsTest, ComplexPolygonCollisions) {
    std::vector<geometry::Point2D> poly_points = {{0, 0}, {2, 0}, {2, 2}, {0, 2}};

    std::vector<geometry::Shape> shapes = {creator.CreatePolygon(poly_points), creator.CreateCircle(1, 1, 0.5),
                                           creator.CreateTriangle(1, 1, 3, 1, 2, 3)};

    auto collisions = FindAllCollisions(std::move(shapes));

    EXPECT_EQ(collisions.size(), 3);
}

TEST_F(FindAllCollisionsTest, RegularPolygonCollisions) {
    std::vector<geometry::Shape> shapes = {creator.CreateRegularPolygon(0, 0, 2, 6), creator.CreateCircle(1, 0, 1),
                                           creator.CreateRectangle(-1, -1, 2, 2)};

    auto collisions = FindAllCollisions(std::move(shapes));

    EXPECT_EQ(collisions.size(), 3);
}

TEST_F(FindAllCollisionsTest, PerformanceWithMultipleShapes) {
    std::vector<geometry::Shape> shapes;

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 2; ++j) {
            if ((i + j) % 2 == 0) {
                shapes.push_back(creator.CreateRectangle(i * 2, j * 2, 1.5, 1.5));
            } else {
                shapes.push_back(creator.CreateCircle(i * 2, j * 2, 0.8));
            }
        }
    }

    auto collisions = FindAllCollisions(std::move(shapes));

    for (const auto &collision : collisions) {
        EXPECT_TRUE(BoundingBoxesOverlap(collision.first, collision.second));
    }
}

TEST_F(FindAllCollisionsTest, NoFalsePositives) {
    std::vector<geometry::Shape> shapes = {creator.CreateRectangle(0, 0, 1, 1), creator.CreateCircle(5, 5, 1),
                                           creator.CreateTriangle(10, 10, 12, 10, 11, 12)};

    auto collisions = FindAllCollisions(std::move(shapes));

    EXPECT_TRUE(collisions.empty());
}

class FindHighestShapeTest : public ::testing::Test {
protected:
    TestShapeCreator creator;
};

TEST_F(FindHighestShapeTest, EmptyVectorReturnsNullopt) {
    std::vector<geometry::Shape> shapes;
    auto result = FindHighestShape(std::move(shapes));
    EXPECT_FALSE(result.has_value());
}

TEST_F(FindHighestShapeTest, SingleShapeReturnsItsHeight) {
    std::vector<geometry::Shape> shapes = {creator.CreateRectangle(0, 0, 10, 5)};

    auto result = FindHighestShape(std::move(shapes));
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(geometry::queries::GetHeight(result.value()), 5.0);
}

TEST_F(FindHighestShapeTest, MultipleShapesReturnsHighest) {
    std::vector<geometry::Shape> shapes = {creator.CreateRectangle(0, 0, 5, 3), creator.CreateCircle(0, 0, 2),
                                           creator.CreateRectangle(0, 0, 2, 8),
                                           creator.CreateTriangle(0, 0, 4, 0, 2, 6)};

    auto result = FindHighestShape(std::move(shapes));
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(geometry::queries::GetHeight(result.value()), 8.0);
}

TEST_F(FindHighestShapeTest, AllShapesSameHeightReturnsThatHeight) {
    std::vector<geometry::Shape> shapes = {creator.CreateRectangle(0, 0, 3, 5), creator.CreateRectangle(0, 0, 2, 5),
                                           creator.CreateCircle(0, 2.5, 2.5)};

    auto result = FindHighestShape(std::move(shapes));
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(geometry::queries::GetHeight(result.value()), 5.0);
}

TEST_F(FindHighestShapeTest, MixedShapeTypesWithDifferentHeights) {
    std::vector<geometry::Shape> shapes = {creator.CreateLine(0, 0, 3, 4), creator.CreateRegularPolygon(0, 0, 3, 6),
                                           creator.CreateCircle(0, 0, 5), creator.CreateTriangle(0, 0, 6, 0, 3, 7)};

    auto result = FindHighestShape(std::move(shapes));
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(geometry::queries::GetHeight(result.value()), 7.0);
}

TEST_F(FindHighestShapeTest, NegativeCoordinates) {
    std::vector<geometry::Shape> shapes = {creator.CreateRectangle(-5, -5, 3, 3), creator.CreateCircle(-2, -1, 4),
                                           creator.CreateTriangle(-3, -10, 1, -10, -1, -4)};

    auto result = FindHighestShape(std::move(shapes));
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(geometry::queries::GetHeight(result.value()), 3.0);
}

TEST_F(FindHighestShapeTest, ComplexPolygonHeight) {
    std::vector<geometry::Point2D> poly_points = {{0, 0}, {3, 0}, {3, 8}, {1, 12}, {0, 8}};

    std::vector<geometry::Shape> shapes = {creator.CreatePolygon(poly_points), creator.CreateRectangle(0, 0, 5, 10),
                                           creator.CreateCircle(0, 0, 6)};

    auto result = FindHighestShape(std::move(shapes));
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(geometry::queries::GetHeight(result.value()), 12.0);
}

TEST_F(FindHighestShapeTest, UsesGetHeightFunctionCorrectly) {
    auto rect1 = creator.CreateRectangle(0, 0, 5, 3);
    auto rect2 = creator.CreateRectangle(0, 0, 2, 7);

    std::vector<geometry::Shape> shapes = {rect1, rect2};

    auto result = FindHighestShape(std::move(shapes));
    ASSERT_TRUE(result.has_value());

    double manual_max_height = std::max(geometry::queries::GetHeight(rect1), geometry::queries::GetHeight(rect2));
    EXPECT_DOUBLE_EQ(geometry::queries::GetHeight(result.value()), manual_max_height);
}