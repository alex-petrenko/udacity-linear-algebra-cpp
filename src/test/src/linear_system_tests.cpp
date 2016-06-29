#include <gtest/gtest.h>

#include <linal/plane3d.hpp>
#include <linal/linear_system.hpp>


using namespace Linal;


TEST(linearSystem, testCtor)
{
    LinearSystem<int> sys
    {
        {{1, 1, 1}, 1},
        {{2, 2, 2}, 2},
    };
}

TEST(linearSystem, testAccess)
{
    const LinearSystem<int> csys
    {
        { { 1, 1, 1 }, 1 },
        { { 2, 2, 2 }, 2 },
    };
    EXPECT_TRUE(csys[0].normVector().equalTo({ 1, 1, 1 }) && csys[0].constTerm() == 1);
    LinearSystem<int> sys{ csys };
    sys[1] = LinearEquation<int>{ {1, 2, 3 }, 4 };
    EXPECT_TRUE(sys[1].normVector().equalTo({ 1, 2, 3 }) && sys[1].constTerm() == 4);
}

TEST(linearSystem, testRowOperations)
{
    // udacity test cases
    Plane3d p0{ {1, 1, 1}, 1 }, p1{ {0, 1, 0}, 2 }, p2{ {1, 1, -1}, 3 }, p3{ {1, 0, -2}, 2 };

    LinearSystem<double> s{ p0, p1, p2, p3 };
    s.swapRows(0, 1);
    EXPECT_TRUE(s[0] == p1 && s[1] == p0 && s[2] == p2 && s[3] == p3);
    s.swapRows(1, 3);
    EXPECT_TRUE(s[0] == p1 && s[1] == p3 && s[2] == p2 && s[3] == p0);
    s.swapRows(3, 1);
    EXPECT_TRUE(s[0] == p1 && s[1] == p0 && s[2] == p2 && s[3] == p3);

    s[0] *= 1;
    EXPECT_TRUE(s[0] == p1 && s[1] == p0 && s[2] == p2 && s[3] == p3);
    s[2] *= -1;
    EXPECT_TRUE(s[0] == p1 && s[1] == p0 && s[2].equalTo({ {-1, -1, 1}, -3 }) && s[3] == p3);
    s[1] *= 10;
    EXPECT_TRUE(s[0] == p1 && s[1].equalTo({ { 10, 10, 10 }, 10 }) && s[2].equalTo({ { -1, -1, 1 }, -3 }) && s[3] == p3);

    s[0].add(s[1], 0);
    EXPECT_TRUE(s[0] == p1 && s[1].equalTo({ { 10, 10, 10 }, 10 }) && s[2].equalTo({ { -1, -1, 1 }, -3 }) && s[3] == p3);
    s[1].add(s[0], 1);
    EXPECT_TRUE(s[0] == p1 && s[1].equalTo({ { 10, 11, 10 }, 12 }) && s[2].equalTo({ { -1, -1, 1 }, -3 }) && s[3] == p3);
    s[0].add(s[1], -1);
    EXPECT_TRUE(s[0].equalTo({ { -10, -10, -10 }, -10 }) && s[1].equalTo({ { 10, 11, 10 }, 12 }) && s[2].equalTo({ { -1, -1, 1 }, -3 }) && s[3] == p3);
}

TEST(linearSystem, testTriangularForm)
{
    {
        const Plane3f p1{ {1.0f, 1.0f, 1.0f}, 1.0f }, p2{ { 0.0f, 1.0f, 1.0f }, 2.0f };
        LinearSystem<float> s{ p1, p2 };
        s.computeTriangularForm();
        EXPECT_TRUE(s[0] == p1 && s[1] == p2);
    }
}
