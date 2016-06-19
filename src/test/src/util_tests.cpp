#include <gtest/gtest.h>

#include <linal/vec.hpp>


using namespace Linal;


TEST(util, testRadToDegrees)
{
    EXPECT_DOUBLE_EQ(radToDegrees(1), 57.295779513082323);
    EXPECT_DOUBLE_EQ(radToDegrees(0), 0);
    EXPECT_DOUBLE_EQ(radToDegrees(0.0), 0);
    EXPECT_FLOAT_EQ(radToDegrees(0.0f), 0.0f);

    static_assert(std::is_same_v<decltype(radToDegrees(float())), float>, "Return type is incorrect!");
}

TEST(util, testSfinae)
{
    static_assert(IsSpecializationOfV<Vec, Vec<int>>, "SFINAE does not work!");
    static_assert(!IsSpecializationOfV<Vec, int>, "SFINAE does not work!");
}
