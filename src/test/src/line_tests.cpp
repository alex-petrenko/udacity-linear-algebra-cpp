#include <gtest/gtest.h>

#include <linal/line.hpp>


using namespace Linal;


TEST(line, testLine)
{
    const Line2i l{ {1, 2}, 3 };
    const Line2d ld({ 10, 20 }, 30);

    EXPECT_EQ(l.normVector(), Veci({ 1, 2 }));
    EXPECT_EQ(l.constTerm(), 3);

    EXPECT_TRUE(ld.normVector().almostEqualTo({ 10, 20 }));
    EXPECT_DOUBLE_EQ(ld.constTerm(), 30);
}

TEST(line, testPnt)
{
    const Line2i l1{ {1, 1}, 0 }, l2{ {10, 0}, 1 }, l3{ {0, 10}, 2 };
    const auto p1 = l1.basepoint(), p2 = l2.basepoint(), p3 = l3.basepoint();
    EXPECT_TRUE(l1.hasPoint(p1));
    EXPECT_TRUE(l2.hasPoint(p2));
    EXPECT_TRUE(l3.hasPoint(p3));
}

TEST(line, testParallel)
{
    const Line2i l1{ {1, 2}, 100 }, l2{ {-100, -200}, 0 };
    EXPECT_TRUE(l1.isParallelTo(l2));
    EXPECT_TRUE(l2.isParallelTo(l1));
    EXPECT_TRUE(l1.isParallelTo(l1));
}

TEST(line, testSame)
{
    const Line2i l1{ {3, 3}, 300 };
    const Line2d l2{ {999, 999}, 300 * 333 };
    EXPECT_TRUE(l1.isSameAs(l2));
    EXPECT_TRUE(l2.isSameAs(l1));
    EXPECT_TRUE(l1.isSameAs(l1));
}

TEST(line, testIntersect)
{
    const Line2f l1{ {1.0f, 1.0f}, 0.0f };
    const Line2i l2{ {-1, 1}, 0 }, l3{ {2, 2}, 0 }, l4{ {2, 2}, 100 };
    EXPECT_EQ(l1.getIntersectionWith(l1).type, IntersectionType::COINCIDE);
    EXPECT_EQ(l1.getIntersectionWith(l1).pnt, nullptr);
    EXPECT_EQ(l1.getIntersectionWith(l2).type, IntersectionType::INTERSECT);
    EXPECT_EQ(*l1.getIntersectionWith(l2).pnt, Vecd({ 0, 0 }));
    EXPECT_EQ(l1.getIntersectionWith(l3).type, IntersectionType::COINCIDE);
    EXPECT_EQ(l1.getIntersectionWith(l4).type, IntersectionType::PARALLEL);
    EXPECT_EQ(l4.getIntersectionWith(l1).type, IntersectionType::PARALLEL);

    // udacity quiz questions
    EXPECT_TRUE(Line2d({ 4.046, 2.836 }, 1.21).getIntersectionType(Line2d({ 10.115, 7.09 }, 3.025)) == IntersectionType::COINCIDE);
    EXPECT_TRUE(Line2d({ 7.204, 3.182 }, 8.68).getIntersectionWith(Line2d({ 8.172, 4.114 }, 9.883)).pnt->almostEqualTo({ 1.172776635, 0.07269551166 }));
    EXPECT_TRUE(Line2d({ 1.182, 5.562 }, 6.744).getIntersectionWith(Line2d({ 1.773, 8.343 }, 9.525)).type == IntersectionType::PARALLEL);
}
