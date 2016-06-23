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
}
