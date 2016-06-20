#include <gtest/gtest.h>

#include <linal/line.hpp>


using namespace Linal;


TEST(line, testLine)
{
    const Linei l{ {1, 2}, 3 };
    const Lined ld({ 10, 20 }, 30);

    EXPECT_EQ(l.normVector(), Veci({ 1, 2 }));
    EXPECT_EQ(l.constTerm(), 3);

    EXPECT_TRUE(ld.normVector().almostEqualTo({ 10, 20 }));
    EXPECT_DOUBLE_EQ(ld.constTerm(), 30);
}

TEST(line, testPnt)
{
    const Linei l1{ {1, 1}, 0 }, l2{ {10, 0}, 1 }, l3{ {0, 10}, 2 };
    const auto p1 = l1.pointOnLine(), p2 = l2.pointOnLine(), p3 = l3.pointOnLine();
    EXPECT_TRUE(pointLiesOnLine(p1, l1));
    EXPECT_TRUE(pointLiesOnLine(p2, l2));
    EXPECT_TRUE(pointLiesOnLine(p3, l3));
}

TEST(line, testParallel)
{
    const Linei l1{ {1, 2}, 100 }, l2{ {-100, -200}, 0 };
    EXPECT_TRUE(l1.isParallelTo(l2));
    EXPECT_TRUE(l2.isParallelTo(l1));
    EXPECT_TRUE(l1.isParallelTo(l1));
}

TEST(line, testSame)
{
    const Linei l1{ {3, 3}, 300 };
    const Lined l2{ {999, 999}, 300 * 333 };
    EXPECT_TRUE(l1.isSameAs(l2));
    EXPECT_TRUE(l2.isSameAs(l1));
    EXPECT_TRUE(l1.isSameAs(l1));
}

TEST(line, testIntersect)
{
    const Linef l1{ {1.0f, 1.0f}, 0.0f };
    const Linei l2{ {-1, 1}, 0 }, l3{ {2, 2}, 0 }, l4{ {2, 2}, 100 };
    EXPECT_EQ(intersection(l1, l1).type, IntersectionType::COINCIDE);
    EXPECT_EQ(intersection(l1, l1).pnt, nullptr);
    EXPECT_EQ(intersection(l1, l2).type, IntersectionType::INTERSECT);
    EXPECT_EQ(*intersection(l1, l2).pnt, Vecd({ 0, 0 }));
    EXPECT_EQ(intersection(l1, l3).type, IntersectionType::COINCIDE);
    EXPECT_EQ(intersection(l1, l4).type, IntersectionType::PARALLEL);
    EXPECT_EQ(intersection(l4, l1).type, IntersectionType::PARALLEL);

    // udacity quiz questions
    EXPECT_TRUE(intersection(Lined({ 4.046, 2.836 }, 1.21), Lined({ 10.115, 7.09 }, 3.025)).type == IntersectionType::COINCIDE);
    EXPECT_TRUE(intersection(Lined({ 7.204, 3.182 }, 8.68), Lined({ 8.172, 4.114 }, 9.883)).pnt->almostEqualTo({ 1.172776635, 0.07269551166 }));
    EXPECT_TRUE(intersection(Lined({ 1.182, 5.562 }, 6.744), Lined({ 1.773, 8.343 }, 9.525)).type == IntersectionType::PARALLEL);
}
