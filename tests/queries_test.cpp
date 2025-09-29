#include "geometry.hpp"
#include <../include/queries.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <print>

const double tolerance = 0.03;

TEST(LineDistanceToPointTest, PointOnLine) {
    geometry::Point2D point{3.0, 3.0};
    geometry::Line line{{1.0, 1.0}, {5.0, 5.0}};

    auto result = geometry::queries::DistanceToPoint(line, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(*result, 0.0, tolerance);
}

TEST(LineDistanceToPointTest, PointPerpendicularToMiddle) {
    geometry::Point2D point{3.0, 3.0};
    geometry::Line line{{0.0, 0.0}, {5.0, 0.0}};

    auto result = geometry::queries::DistanceToPoint(line, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(*result, 3.0, tolerance);
}

TEST(PointToCircleDistanceTest, PointOutsideCircle) {
    geometry::Point2D point{3.0, 4.0};
    geometry::Circle circle{{0.0, 0.0}, 2.0};
    auto result = geometry::queries::DistanceToPoint(circle, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(3.0, *result, tolerance);
}

TEST(PointToCircleDistanceTest, PointInsideCircle) {
    geometry::Point2D point{3.0, 4.0};
    geometry::Circle circle{{0.0, 0.0}, 6.0};
    auto result = geometry::queries::DistanceToPoint(circle, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(1.0, *result, tolerance);
}

TEST(PointToCircleDistanceTest, PointOnCircle) {
    geometry::Point2D point{3.0, 4.0};
    geometry::Circle circle{{0.0, 0.0}, 5.0};
    auto result = geometry::queries::DistanceToPoint(circle, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.0, *result, tolerance);
}

TEST(TriangleDistanceTest, PointInsideTriangle) {
    geometry::Point2D point{2.0, 2.0};
    geometry::Triangle triangle{{0.0, 0.0}, {4.0, 0.0}, {2.0, 4.0}};

    auto result = geometry::queries::DistanceToPoint(triangle, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.0, *result, tolerance);
}

TEST(TriangleDistanceTest, PointOutside) {
    geometry::Point2D point{2.0, -1.0};
    geometry::Triangle triangle{{0.0, 0.0}, {4.0, 0.0}, {2.0, 4.0}};

    auto result = geometry::queries::DistanceToPoint(triangle, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(1.0, *result, tolerance);
}

TEST(TriangleDistanceTest, PointOnTriangleSide) {
    geometry::Point2D point{2.0, 0.0};
    geometry::Triangle triangle{{0.0, 0.0}, {4.0, 0.0}, {2.0, 4.0}};

    auto result = geometry::queries::DistanceToPoint(triangle, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.0, *result, tolerance);
}

TEST(PointToRectangleDistanceTest, PointInsideRectangle) {
    geometry::Point2D point{3.0, 3.0};
    geometry::Rectangle rect{{1.0, 1.0}, 4.0, 4.0};

    auto result = geometry::queries::DistanceToPoint(rect, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.0, *result, tolerance);
}

TEST(PointToRectangleDistanceTest, PointOutsideClosestToSide) {
    geometry::Point2D point{3.0, 0.5};
    geometry::Rectangle rect{{1.0, 1.0}, 4.0, 4.0};

    auto result = geometry::queries::DistanceToPoint(rect, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.5, *result, tolerance);
}

TEST(PointToRectangleDistanceTest, PointOutsideClosestToCorner) {
    geometry::Point2D point{0.0, 0.0};
    geometry::Rectangle rect{{1.0, 1.0}, 4.0, 4.0};

    auto result = geometry::queries::DistanceToPoint(rect, point);
    ASSERT_TRUE(result.has_value());
    double expected = std::sqrt(2.0);
    EXPECT_NEAR(expected, *result, tolerance);
}

TEST(PointToRectangleDistanceTest, PointOnRectangleBorder) {
    geometry::Point2D point{3.0, 1.0};
    geometry::Rectangle rect{{1.0, 1.0}, 4.0, 4.0};

    auto result = geometry::queries::DistanceToPoint(rect, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.0, *result, tolerance);
}

TEST(PointToRegularPolygonDistanceTest, PointAtCenter) {
    geometry::Point2D point{0.0, 0.0};
    geometry::RegularPolygon poly{{0.0, 0.0}, 5.0, 4};

    auto result = geometry::queries::DistanceToPoint(poly, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.0, *result, tolerance);
}

TEST(PointToRegularPolygonDistanceTest, PointOnSide) {
    geometry::Point2D point{2.5, 2.5};
    geometry::RegularPolygon poly{{0.0, 0.0}, 5.0, 4};

    auto result = geometry::queries::DistanceToPoint(poly, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.0, *result, tolerance);
}

TEST(PointToRegularPolygonDistanceTest, PointOutside) {
    geometry::Point2D point{3.0, 3.0};
    geometry::RegularPolygon poly{{0.0, 0.0}, 5.0, 4};

    auto result = geometry::queries::DistanceToPoint(poly, point);
    ASSERT_TRUE(result.has_value());
    double expected = 1.0 / std::sqrt(2.0);
    EXPECT_NEAR(expected, *result, tolerance);
}

TEST(PointToRegularPolygonDistanceTest, HexagonPointOnAxis) {
    geometry::Point2D point{0.0, 5.0};
    geometry::RegularPolygon poly{{0.0, 0.0}, 4.0, 6};

    auto result = geometry::queries::DistanceToPoint(poly, point);
    ASSERT_TRUE(result.has_value());
    double expected = 5.0 - 4.0 * std::sin(M_PI / 3.0);
    EXPECT_NEAR(expected, *result, tolerance);
}

TEST(PointToPolygonDistanceTest, PointInsideConvexPolygon) {
    geometry::Point2D point{1.5, 1.5};
    geometry::Polygon poly{{{0.0, 0.0}, {0.0, 3.0}, {3.0, 3.0}, {3.0, 0.0}}};

    auto result = geometry::queries::DistanceToPoint(poly, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.0, *result, tolerance);
}

TEST(PointToPolygonDistanceTest, PointOnPolygonBorder) {
    geometry::Point2D point{1.5, 0.0};
    geometry::Polygon poly{{{0.0, 0.0}, {0.0, 3.0}, {3.0, 3.0}, {3.0, 0.0}}};

    auto result = geometry::queries::DistanceToPoint(poly, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.0, *result, tolerance);
}

TEST(PointToPolygonDistanceTest, PointOutsideConvexPolygon) {
    geometry::Point2D point{4.0, 1.5};
    geometry::Polygon poly{{{0.0, 0.0}, {0.0, 3.0}, {3.0, 3.0}, {3.0, 0.0}}};

    auto result = geometry::queries::DistanceToPoint(poly, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(1.0, *result, tolerance);
}

TEST(PointToPolygonDistanceTest, PointInConcavePolygonCutout) {
    geometry::Point2D point{1.5, 1.5};
    geometry::Polygon poly{
        {{0.0, 0.0}, {0.0, 3.0}, {3.0, 3.0}, {3.0, 0.0}, {2.0, 0.0}, {2.0, 2.0}, {1.0, 2.0}, {1.0, 0.0}}};

    auto result = geometry::queries::DistanceToPoint(poly, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.5, *result, tolerance);
}

TEST(PointToPolygonDistanceTest, PointOutsideConcavePolygon) {
    geometry::Point2D point{1.5, 0.5};
    geometry::Polygon poly{
        {{0.0, 0.0}, {0.0, 3.0}, {3.0, 3.0}, {3.0, 0.0}, {2.0, 0.0}, {2.0, 2.0}, {1.0, 2.0}, {1.0, 0.0}}};

    auto result = geometry::queries::DistanceToPoint(poly, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.5, *result, tolerance);
}

TEST(PointToPolygonDistanceTest, DegeneratePolygonPoint) {
    geometry::Point2D point{1.0, 1.0};
    geometry::Polygon poly{{{2.0, 2.0}}};

    auto result = geometry::queries::DistanceToPoint(poly, point);
    ASSERT_TRUE(result.has_value());
    double expected = std::sqrt(2.0);
    EXPECT_NEAR(expected, *result, tolerance);
}

TEST(PointToPolygonDistanceTest, DegeneratePolygonLine) {
    geometry::Point2D point{1.0, 1.0};
    geometry::Polygon poly{{{0.0, 0.0}, {2.0, 2.0}}};

    auto result = geometry::queries::DistanceToPoint(poly, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.0, *result, tolerance);
}

TEST(PointToPolygonDistanceTest, ComplexPolygon) {
    geometry::Point2D point{2.0, 1.0};
    geometry::Polygon poly{{{2.0, 0.0},
                            {3.0, 2.0},
                            {5.0, 2.0},
                            {3.5, 3.5},
                            {4.0, 5.0},
                            {2.0, 4.0},
                            {0.0, 5.0},
                            {0.5, 3.5},
                            {-1.0, 2.0},
                            {1.0, 2.0}}};

    auto result = geometry::queries::DistanceToPoint(poly, point);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.0, *result, tolerance);
}

TEST(DistanceBetweenShapesTest, CirclesSeparate) {
    geometry::Circle circle1{{0.0, 0.0}, 2.0};
    geometry::Circle circle2{{6.0, 0.0}, 2.0};

    auto result = geometry::queries::DistanceBetweenShapes(circle1, circle2);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(2.0, result.value(), tolerance);
}

TEST(DistanceBetweenShapesTest, CirclesIntersecting) {
    geometry::Circle circle1{{0.0, 0.0}, 3.0};
    geometry::Circle circle2{{4.0, 0.0}, 2.0};

    auto result = geometry::queries::DistanceBetweenShapes(circle1, circle2);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.0, result.value(), tolerance);
}

TEST(DistanceBetweenShapesTest, CirclesTangent) {
    geometry::Circle circle1{{0.0, 0.0}, 2.0};
    geometry::Circle circle2{{4.0, 0.0}, 2.0};

    auto result = geometry::queries::DistanceBetweenShapes(circle1, circle2);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.0, result.value(), tolerance);
}

TEST(DistanceBetweenShapesTest, CirclesOneInsideAnother) {
    geometry::Circle circle1{{0.0, 0.0}, 5.0};
    geometry::Circle circle2{{0.0, 0.0}, 2.0};

    auto result = geometry::queries::DistanceBetweenShapes(circle1, circle2);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(0.0, result.value(), tolerance);
}

TEST(DistanceBetweenShapesTest, LinesParallel) {
    geometry::Line line1{{0.0, 0.0}, {5.0, 0.0}};
    geometry::Line line2{{0.0, 3.0}, {5.0, 3.0}};

    auto result = geometry::queries::DistanceBetweenShapes(line1, line2);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(3.0, result.value(), tolerance);
}

TEST(DistanceBetweenShapesTest, LinesPerpendicularClose) {
    geometry::Line line1{{0.0, 0.0}, {5.0, 0.0}};
    geometry::Line line2{{2.0, 1.0}, {2.0, 3.0}};

    auto result = geometry::queries::DistanceBetweenShapes(line1, line2);
    ASSERT_TRUE(result.has_value());
    double expected = std::sqrt(5.0);
    EXPECT_NEAR(expected, result.value(), tolerance);
}

TEST(DistanceBetweenShapesTest, LinesCollinearNotOverlapping) {
    geometry::Line line1{{0.0, 0.0}, {2.0, 0.0}};
    geometry::Line line2{{3.0, 0.0}, {5.0, 0.0}};

    auto result = geometry::queries::DistanceBetweenShapes(line1, line2);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(1.0, result.value(), tolerance);
}

TEST(DistanceBetweenShapesTest, LineAndCircle) {
    geometry::Line line{{0.0, 0.0}, {5.0, 0.0}};
    geometry::Circle circle{{0.0, 0.0}, 2.0};

    auto result = geometry::queries::DistanceBetweenShapes(line, circle);
    EXPECT_FALSE(result.has_value());
}

TEST(DistanceBetweenShapesTest, CircleAndLine) {
    geometry::Circle circle{{0.0, 0.0}, 2.0};
    geometry::Line line{{0.0, 0.0}, {5.0, 0.0}};

    auto result = geometry::queries::DistanceBetweenShapes(circle, line);
    EXPECT_FALSE(result.has_value());
}

TEST(DistanceBetweenShapesTest, RectangleAndRectangle) {
    geometry::Rectangle rect1{{0.0, 0.0}, 2.0, 2.0};
    geometry::Rectangle rect2{{3.0, 3.0}, 2.0, 2.0};

    auto result = geometry::queries::DistanceBetweenShapes(rect1, rect2);
    EXPECT_FALSE(result.has_value());
}

TEST(DistanceBetweenShapesTest, TriangleAndCircle) {
    geometry::Triangle triangle{{0.0, 0.0}, {3.0, 0.0}, {1.5, 2.0}};
    geometry::Circle circle{{4.0, 4.0}, 1.0};

    auto result = geometry::queries::DistanceBetweenShapes(triangle, circle);
    EXPECT_FALSE(result.has_value());
}

TEST(DistanceBetweenShapesTest, ZeroLengthLine) {
    geometry::Line line1{{1.0, 1.0}, {1.0, 1.0}};
    geometry::Line line2{{2.0, 2.0}, {3.0, 3.0}};

    auto result = geometry::queries::DistanceBetweenShapes(line1, line2);
    ASSERT_TRUE(result.has_value());
    double expected = std::sqrt(2.0);
    EXPECT_NEAR(expected, result.value(), tolerance);
}

TEST(DistanceBetweenShapesTest, BothZeroLengthLines) {
    geometry::Line line1{{1.0, 1.0}, {1.0, 1.0}};
    geometry::Line line2{{2.0, 2.0}, {2.0, 2.0}};

    auto result = geometry::queries::DistanceBetweenShapes(line1, line2);
    ASSERT_TRUE(result.has_value());
    double expected = std::sqrt(2.0);
    EXPECT_NEAR(expected, result.value(), tolerance);
}

TEST(DistanceBetweenShapesTest, ZeroRadiusCircles) {
    geometry::Circle circle1{{0.0, 0.0}, 0.0};
    geometry::Circle circle2{{3.0, 0.0}, 0.0};

    auto result = geometry::queries::DistanceBetweenShapes(circle1, circle2);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(3.0, result.value(), tolerance);
}

TEST(DistanceBetweenShapesTest, SymmetryTest) {
    geometry::Circle circle1{{0.0, 0.0}, 2.0};
    geometry::Circle circle2{{5.0, 0.0}, 1.0};

    auto result1 = geometry::queries::DistanceBetweenShapes(circle1, circle2);
    auto result2 = geometry::queries::DistanceBetweenShapes(circle2, circle1);

    EXPECT_EQ(result1.has_value(), result2.has_value());
    if (result1.has_value() && result2.has_value()) {
        EXPECT_NEAR(result1.value(), result2.value(), tolerance);
    }
}
