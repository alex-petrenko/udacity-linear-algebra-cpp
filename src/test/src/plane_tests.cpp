#include <gtest/gtest.h>

#include <linal/plane.hpp>


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
