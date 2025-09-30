#include "geometry.hpp"
#include <../include/intersections.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <print>

const double tolerance = 0.03;

TEST(IntersectionTest, TwoParallelLinesNoIntersectionTest) {
    geometry::Point2D a{10.0, 20.0};
    geometry::Point2D b{30.0, 40.0};
    geometry::Point2D c{50.0, 60.0};
    geometry::Point2D d{70.0, 80.0};

    geometry::Line line1{a, b};
    geometry::Line line2{c, d};

    auto res = geometry::intersections::GetIntersectPoint(line1, line2);

    ASSERT_TRUE(!res);
}

TEST(IntersectionTest, TwoLinesNoIntersectionTest) {
    geometry::Point2D a{10.0, 20.0};
    geometry::Point2D b{30.0, 40.0};
    geometry::Point2D c{50.0, 60.0};
    geometry::Point2D d{70.0, 90.0};

    geometry::Line line1{a, b};
    geometry::Line line2{c, d};

    auto res = geometry::intersections::GetIntersectPoint(line1, line2);

    ASSERT_TRUE(!res);
}

TEST(IntersectionTest, TwoLinesIntersectionTest) {
    geometry::Point2D a{5.0, 20.0};
    geometry::Point2D b{30.0, 10.0};
    geometry::Point2D c{0, 7.5};
    geometry::Point2D d{23, 19.5};

    geometry::Line line1{a, b};
    geometry::Line line2{c, d};

    auto res = geometry::intersections::GetIntersectPoint(line1, line2);

    ASSERT_TRUE(res && !res.value().empty());
    geometry::Point2D intersect_point = res.value()[0];
    EXPECT_NEAR(intersect_point.x, 15.76, tolerance);
    EXPECT_NEAR(intersect_point.y, 15.7, tolerance);
}

::testing::AssertionResult PointsNear(const geometry::Point2D &expected, const geometry::Point2D &actual,
                                      double tolerance = 1e-10) {
    if (std::abs(expected.x - actual.x) < tolerance && std::abs(expected.y - actual.y) < tolerance) {
        return ::testing::AssertionSuccess();
    } else {
        return ::testing::AssertionFailure() << "Expected: (" << expected.x << ", " << expected.y << ")\n"
                                             << "Actual: (" << actual.x << ", " << actual.y << ")";
    }
}

TEST(LineCircleIntersectionTest, NoIntersection) {
    geometry::Line line{{5.0, 5.0}, {10.0, 10.0}};
    geometry::Circle circle{{0.0, 0.0}, 1.0};

    auto result = geometry::intersections::GetIntersectPoint(line, circle);
    EXPECT_FALSE(result.has_value());
}

TEST(LineCircleIntersectionTest, TwoIntersections) {
    geometry::Line line{{-2.0, 0.0}, {2.0, 0.0}};
    geometry::Circle circle{{0.0, 0.0}, 1.0};

    auto result = geometry::intersections::GetIntersectPoint(line, circle);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->size(), 2);

    geometry::Point2D p1{-1.0, 0.0};
    geometry::Point2D p2{1.0, 0.0};

    EXPECT_TRUE((PointsNear(p1, result->at(0)) && PointsNear(p2, result->at(1))) ||
                (PointsNear(p2, result->at(0)) && PointsNear(p1, result->at(1))));
}

TEST(CircleCircleIntersectionTest, NoIntersection) {
    geometry::Circle circle1{{0.0, 0.0}, 1.0};
    geometry::Circle circle2{{3.0, 0.0}, 1.0};

    auto result = geometry::intersections::GetIntersectPoint(circle1, circle2);
    EXPECT_FALSE(result.has_value());
}

TEST(CircleCircleIntersectionTest, TwoIntersections) {
    geometry::Circle circle1{{0.0, 0.0}, 2.0};
    geometry::Circle circle2{{3.0, 0.0}, 2.0};

    auto result = geometry::intersections::GetIntersectPoint(circle1, circle2);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->size(), 2);

    geometry::Point2D p1{1.5, std::sqrt(4 - 2.25)};
    geometry::Point2D p2{1.5, -std::sqrt(4 - 2.25)};

    EXPECT_TRUE((PointsNear(p1, result->at(0)) && PointsNear(p2, result->at(1))) ||
                (PointsNear(p2, result->at(0)) && PointsNear(p1, result->at(1))));
}

TEST(CircleCircleIntersectionTest, ConcentricCircles) {
    geometry::Circle circle1{{0.0, 0.0}, 1.0};
    geometry::Circle circle2{{0.0, 0.0}, 2.0};

    auto result = geometry::intersections::GetIntersectPoint(circle1, circle2);
    EXPECT_FALSE(result.has_value());
}

TEST(CircleCircleIntersectionTest, SameCircle) {
    geometry::Circle circle1{{0.0, 0.0}, 1.0};
    geometry::Circle circle2{{0.0, 0.0}, 1.0};

    auto result = geometry::intersections::GetIntersectPoint(circle1, circle2);
    ASSERT_FALSE(result.has_value());
}

TEST(EdgeCaseTest, ZeroLengthLine) {
    geometry::Line line{{1.0, 0.0}, {1.0, 0.0}};
    geometry::Circle circle{{0.0, 0.0}, 1.0};

    auto result = geometry::intersections::GetIntersectPoint(line, circle);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->size(), 1);
    EXPECT_TRUE(PointsNear({1.0, 0.0}, result->at(0)));
}

TEST(EdgeCaseTest, ZeroRadiusCircle) {
    geometry::Line line{{-1.0, 0.0}, {1.0, 0.0}};
    geometry::Circle circle{{0.5, 0.0}, 0.0};

    auto result = geometry::intersections::GetIntersectPoint(line, circle);
    ASSERT_TRUE(result.has_value());
}

TEST(EdgeCaseTest, ZeroRadiusCircleNoIntersection) {
    geometry::Line line{{-1.0, 0.0}, {1.0, 0.0}};
    geometry::Circle circle{{0.0, 0.5}, 0.0};

    auto result = geometry::intersections::GetIntersectPoint(line, circle);
    ASSERT_FALSE(result.has_value());
}

TEST(EdgeCaseTest, LineThroughCircleCenter) {
    geometry::Line line{{0.0, -2.0}, {0.0, 2.0}};
    geometry::Circle circle{{0.0, 0.0}, 1.0};

    auto result = geometry::intersections::GetIntersectPoint(line, circle);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->size(), 2);

    geometry::Point2D p1{0.0, 1.0};
    geometry::Point2D p2{0.0, -1.0};

    EXPECT_TRUE((PointsNear(p1, result->at(0)) && PointsNear(p2, result->at(1))) ||
                (PointsNear(p2, result->at(0)) && PointsNear(p1, result->at(1))));
}

TEST(SymmetryTest, LineCircleCommutative) {
    geometry::Line line{{-2.0, 0.0}, {2.0, 0.0}};
    geometry::Circle circle{{0.0, 0.0}, 1.0};

    auto result1 = geometry::intersections::GetIntersectPoint(line, circle);
    auto result2 = geometry::intersections::GetIntersectPoint(circle, line);

    EXPECT_EQ(result1.has_value(), result2.has_value());
    if (result1.has_value() && result2.has_value()) {
        EXPECT_EQ(result1->size(), result2->size());
    }
}