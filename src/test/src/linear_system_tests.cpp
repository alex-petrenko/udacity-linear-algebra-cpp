#include <gtest/gtest.h>

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
