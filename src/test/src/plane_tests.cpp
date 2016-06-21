#include <gtest/gtest.h>

#include <linal/plane3d.hpp>


using namespace Linal;


TEST(plane, testPlane)
{
    const Plane3i pl{ {1, 2, 3}, 3 };
    const Plane3d pld({ 10, 20, 30 }, 30);

    EXPECT_EQ(pl.normVector(), Veci({ 1, 2, 3 }));
    EXPECT_EQ(pl.constTerm(), 3);

    EXPECT_TRUE(pld.normVector().almostEqualTo({ 10, 20, 30 }));
    EXPECT_DOUBLE_EQ(pld.constTerm(), 30);
}

TEST(plane, testIntersect)
{
    const Plane3i p1{ {0, 1, 0}, 0 }, p2{ {0, 10, 0}, 100 }, p3{ {1, 0, 0}, -10 };
    EXPECT_TRUE(p1.getIntersectionType(p1) == IntersectionType::COINCIDE);
    EXPECT_TRUE(p2.getIntersectionType(p1) == IntersectionType::PARALLEL);
    EXPECT_TRUE(p1.getIntersectionType(p2) == IntersectionType::PARALLEL);
    EXPECT_TRUE(p3.getIntersectionType(p1) == IntersectionType::INTERSECT);

    // udacity quiz questions
    EXPECT_EQ(Plane3f({ -0.412f, 3.806f, 0.728f }, -3.46f).getIntersectionType(Plane3d({ 1.03, -9.515, -1.82 }, 8.65)), IntersectionType::COINCIDE);
    EXPECT_EQ(Plane3d({ 2.611, 5.528, 0.283 }, 4.6).getIntersectionType(Plane3d({ 7.715, 8.306, 5.342 }, 3.76)), IntersectionType::INTERSECT);
    EXPECT_EQ(Plane3d({ -7.926, 8.625, -7.217 }, -7.952).getIntersectionType(Plane3d({ -2.642, 2.875, -2.404 }, -2.443)), IntersectionType::PARALLEL);
}
