#include <../include/geometry.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <initializer_list>

const double tolerance = 0.01;

TEST(GeometryTest, TriangleTests) {
    geometry::Point2D a{50.0, 23.0};
    geometry::Point2D b{10.0, 11.0};
    geometry::Point2D c{33.0, 21.0};

    geometry::Triangle tr{a, b, c};

    geometry::Point2D center = tr.Center();
    EXPECT_NEAR(center.x, 31., tolerance);
    EXPECT_NEAR(center.y, 18.33, tolerance);

    double area = tr.Area();
    EXPECT_NEAR(area, 62, tolerance);

    double height = tr.Height();
    EXPECT_NEAR(height, 23, tolerance);

    geometry::BoundingBox box = tr.BoundBox();
    EXPECT_NEAR(box.min_x, 10, tolerance);
    EXPECT_NEAR(box.min_y, 11, tolerance);
    EXPECT_NEAR(box.max_x, 50, tolerance);
    EXPECT_NEAR(box.max_y, 23, tolerance);

    auto vertices = tr.Vertices();
    ASSERT_EQ(vertices.size(), 3u);
    ASSERT_EQ(vertices[0], a);
    ASSERT_EQ(vertices[1], b);
    ASSERT_EQ(vertices[2], c);

    auto lines = tr.Lines();
    ASSERT_EQ(lines.x.size(), 4u);
    ASSERT_EQ(lines.y.size(), 4u);

    EXPECT_NEAR(lines.x[0], 50, tolerance);
    EXPECT_NEAR(lines.y[0], 23, tolerance);

    EXPECT_NEAR(lines.x[1], 10, tolerance);
    EXPECT_NEAR(lines.y[1], 11, tolerance);

    EXPECT_NEAR(lines.x[2], 33, tolerance);
    EXPECT_NEAR(lines.y[2], 21, tolerance);

    EXPECT_NEAR(lines.x[3], 50, tolerance);
    EXPECT_NEAR(lines.y[3], 23, tolerance);
}

TEST(GeometryTest, RectangleTest) {
    geometry::Point2D bl{50.0, 23.0};
    double width = 100.0;
    double height = 50.0;
    geometry::Rectangle rect{bl, width, height};

    geometry::Point2D center = rect.Center();
    EXPECT_NEAR(center.x, 100, tolerance);
    EXPECT_NEAR(center.y, 48, tolerance);

    double height_ = rect.Height();
    EXPECT_NEAR(height_, 73, tolerance);

    double area = rect.Area();
    EXPECT_NEAR(area, 5000, tolerance);

    geometry::BoundingBox box = rect.BoundBox();
    EXPECT_NEAR(box.min_x, 50, tolerance);
    EXPECT_NEAR(box.min_y, 23, tolerance);
    EXPECT_NEAR(box.max_x, 150, tolerance);
    EXPECT_NEAR(box.max_y, 73, tolerance);

    auto vertices = rect.Vertices();
    ASSERT_EQ(vertices.size(), 4u);
    ASSERT_EQ(vertices[0], (geometry::Point2D{50, 23}));
    ASSERT_EQ(vertices[1], (geometry::Point2D{50, 73}));
    ASSERT_EQ(vertices[2], (geometry::Point2D{150, 73}));
    ASSERT_EQ(vertices[3], (geometry::Point2D{150, 23}));

    auto lines = rect.Lines();
    ASSERT_EQ(lines.x.size(), 5u);
    ASSERT_EQ(lines.y.size(), 5u);

    EXPECT_NEAR(lines.x[0], 50, tolerance);
    EXPECT_NEAR(lines.y[0], 23, tolerance);

    EXPECT_NEAR(lines.x[1], 50, tolerance);
    EXPECT_NEAR(lines.y[1], 73, tolerance);

    EXPECT_NEAR(lines.x[2], 150, tolerance);
    EXPECT_NEAR(lines.y[2], 73, tolerance);

    EXPECT_NEAR(lines.x[3], 150, tolerance);
    EXPECT_NEAR(lines.y[3], 23, tolerance);

    EXPECT_NEAR(lines.x[4], 50, tolerance);
    EXPECT_NEAR(lines.y[4], 23, tolerance);
}

/*
Table with coordinates of vertices of polygon with
radius = 100
center = {10.0, 20.0}
N = 4

{110, 20}
{10, 120}
{-90, 20}
{10, -80}
*/

TEST(GeometryTest, CircleTest) {
    geometry::Point2D cc{10.0, 20.0};
    double radius = 100.0;
    geometry::Circle circle{cc, radius};

    geometry::Point2D center = circle.Center();
    EXPECT_NEAR(center.x, 10, tolerance);
    EXPECT_NEAR(center.y, 20, tolerance);

    double height = circle.Height();
    EXPECT_NEAR(height, 120, tolerance);

    geometry::BoundingBox box = circle.BoundBox();
    EXPECT_NEAR(box.min_x, -90, tolerance);
    EXPECT_NEAR(box.min_y, -80, tolerance);
    EXPECT_NEAR(box.max_x, 110, tolerance);
    EXPECT_NEAR(box.max_y, 120, tolerance);

    size_t N = 4u;
    auto vertices = circle.Vertices(N);
    ASSERT_EQ(vertices.size(), N);

    EXPECT_NEAR(vertices[0].x, 110, tolerance);
    EXPECT_NEAR(vertices[0].y, 20, tolerance);

    EXPECT_NEAR(vertices[1].x, 10, tolerance);
    EXPECT_NEAR(vertices[1].y, 120, tolerance);

    EXPECT_NEAR(vertices[2].x, -90, tolerance);
    EXPECT_NEAR(vertices[2].y, 20, tolerance);

    EXPECT_NEAR(vertices[3].x, 10, tolerance);
    EXPECT_NEAR(vertices[3].y, -80, tolerance);

    auto lines = circle.Lines(N);
    ASSERT_EQ(lines.x.size(), N + 1);
    ASSERT_EQ(lines.y.size(), N + 1);

    EXPECT_NEAR(lines.x[0], 110, tolerance);
    EXPECT_NEAR(lines.y[0], 20, tolerance);

    EXPECT_NEAR(lines.x[1], 10, tolerance);
    EXPECT_NEAR(lines.y[1], 120, tolerance);

    EXPECT_NEAR(lines.x[2], -90, tolerance);
    EXPECT_NEAR(lines.y[2], 20, tolerance);

    EXPECT_NEAR(lines.x[3], 10, tolerance);
    EXPECT_NEAR(lines.y[3], -80, tolerance);

    EXPECT_NEAR(lines.x[4], 110, tolerance);
    EXPECT_NEAR(lines.y[4], 20, tolerance);
}

TEST(GeometryTest, LineTest) {
    geometry::Point2D start{10.0, 20.0};
    geometry::Point2D end{100.0, 50.0};

    geometry::Line line{start, end};

    /*
    L = sqrt(dx^2 + dy^2) = sqrt(8100 + 900) = 94.86
    */
    double length = line.Length();
    EXPECT_NEAR(length, 94.86, tolerance);

    geometry::Point2D center = line.Center();
    EXPECT_NEAR(center.x, 55, tolerance);
    EXPECT_NEAR(center.y, 35, tolerance);

    double height = line.Height();
    EXPECT_NEAR(height, std::max(start.y, end.y), tolerance);

    geometry::BoundingBox box = line.BoundBox();

    EXPECT_NEAR(box.min_x, 10, tolerance);
    EXPECT_NEAR(box.min_y, 20, tolerance);
    EXPECT_NEAR(box.max_x, 100, tolerance);
    EXPECT_NEAR(box.max_y, 50, tolerance);

    auto vertices = line.Vertices();
    ASSERT_EQ(vertices.size(), 2u);
    ASSERT_EQ(vertices[0], start);
    ASSERT_EQ(vertices[1], end);

    auto lines = line.Lines();
    ASSERT_EQ(lines.x.size(), 2u);
    ASSERT_EQ(lines.y.size(), 2u);

    ASSERT_EQ(lines.x[0], start.x);
    ASSERT_EQ(lines.y[0], start.y);

    ASSERT_EQ(lines.x[1], end.x);
    ASSERT_EQ(lines.y[1], end.y);
}

TEST(GeometryTest, RegularPolygonTest) {
    geometry::Point2D rpc{10.0, 20.0};
    double radius = 100.0;
    int sides = 4;
    geometry::RegularPolygon regpol{rpc, radius, sides};

    geometry::Point2D center = regpol.Center();
    EXPECT_NEAR(center.x, 10, tolerance);
    EXPECT_NEAR(center.y, 20, tolerance);

    double height = regpol.Height();
    EXPECT_NEAR(height, 120, tolerance);

    geometry::BoundingBox box = regpol.BoundBox();
    EXPECT_NEAR(box.min_x, -90, tolerance);
    EXPECT_NEAR(box.min_y, -80, tolerance);
    EXPECT_NEAR(box.max_x, 110, tolerance);
    EXPECT_NEAR(box.max_y, 120, tolerance);

    auto vertices = regpol.Vertices();
    ASSERT_EQ(vertices.size(), sides);

    EXPECT_NEAR(vertices[0].x, 110, tolerance);
    EXPECT_NEAR(vertices[0].y, 20, tolerance);

    EXPECT_NEAR(vertices[1].x, 10, tolerance);
    EXPECT_NEAR(vertices[1].y, 120, tolerance);

    EXPECT_NEAR(vertices[2].x, -90, tolerance);
    EXPECT_NEAR(vertices[2].y, 20, tolerance);

    EXPECT_NEAR(vertices[3].x, 10, tolerance);
    EXPECT_NEAR(vertices[3].y, -80, tolerance);

    auto lines = regpol.Lines();
    ASSERT_EQ(lines.x.size(), sides + 1);
    ASSERT_EQ(lines.y.size(), sides + 1);

    EXPECT_NEAR(lines.x[0], 110, tolerance);
    EXPECT_NEAR(lines.y[0], 20, tolerance);

    EXPECT_NEAR(lines.x[1], 10, tolerance);
    EXPECT_NEAR(lines.y[1], 120, tolerance);

    EXPECT_NEAR(lines.x[2], -90, tolerance);
    EXPECT_NEAR(lines.y[2], 20, tolerance);

    EXPECT_NEAR(lines.x[3], 10, tolerance);
    EXPECT_NEAR(lines.y[3], -80, tolerance);

    EXPECT_NEAR(lines.x[4], 110, tolerance);
    EXPECT_NEAR(lines.y[4], 20, tolerance);
}

TEST(GeometryTest, PolygonTest) {
    std::vector<geometry::Point2D> points{{50.0, 23.0}, {10.0, 11.0}, {33.0, 21.0}};

    geometry::Polygon pol{points};

    geometry::Point2D center_ = std::ranges::fold_left(points, {}, std::plus<>{});
    center_ = center_ / static_cast<double>(std::ranges::distance(points));
    geometry::Point2D center = pol.Center();
    ASSERT_EQ(center, center_);

    double height = pol.Height();
    EXPECT_NEAR(height, 23.0, tolerance);

    geometry::BoundingBox box = pol.BoundBox();
    EXPECT_NEAR(box.min_x, 10., tolerance);
    EXPECT_NEAR(box.min_y, 11., tolerance);
    EXPECT_NEAR(box.max_x, 50., tolerance);
    EXPECT_NEAR(box.max_y, 23., tolerance);

    auto vertices = pol.Vertices();
    ASSERT_EQ(vertices.size(), 3);
    ASSERT_EQ(vertices[0], (geometry::Point2D{50.0, 23.0}));
    ASSERT_EQ(vertices[1], (geometry::Point2D{10.0, 11.0}));
    ASSERT_EQ(vertices[2], (geometry::Point2D{33.0, 21.0}));

    auto lines = pol.Lines();
    ASSERT_EQ(lines.x.size(), 4);
    ASSERT_EQ(lines.y.size(), 4);

    EXPECT_NEAR(lines.x[0], 50, tolerance);
    EXPECT_NEAR(lines.y[0], 23, tolerance);

    EXPECT_NEAR(lines.x[1], 10, tolerance);
    EXPECT_NEAR(lines.y[1], 11, tolerance);

    EXPECT_NEAR(lines.x[2], 33, tolerance);
    EXPECT_NEAR(lines.y[2], 21, tolerance);

    EXPECT_NEAR(lines.x[3], 50, tolerance);
    EXPECT_NEAR(lines.y[3], 23, tolerance);
}

TEST(GeometryTest, FormatterType) {
    std::vector<geometry::Point2D> points{{50.0, 23.0}, {10.0, 11.0}, {33.0, 21.0}};

    std::string result = std::format("{}", points);
    EXPECT_EQ(result, "{50.0, 23.0}, {10.0, 11.0}, {33.0, 21.0}");

    std::string result2 = std::format("{:new_line}", points);
    EXPECT_EQ(result2, "\t{50.0, 23.0}\n\t{10.0, 11.0}\n\t{33.0, 21.0}");
}