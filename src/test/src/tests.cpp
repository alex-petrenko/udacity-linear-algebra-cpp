#include <gtest/gtest.h>

#include <linal/vec.hpp>


using namespace Linal;


TEST(vec, testBasic)
{
    {
        Vec<int> v;
        EXPECT_EQ(v.ndim(), 0);
    }

    {
        Vec<float> v{ 1.0f, 2.0f, 3.0f };
        EXPECT_EQ(v.ndim(), 3);
    }

    {
        Vec<long long> v{ 1, 10, 100, 1000 }, w{ 1, 10, 100, 1000 }, u{ 2, 3, 4, 5 };
        EXPECT_EQ(v, w);
        EXPECT_NE(v, u);
        EXPECT_NE(w, u);
        EXPECT_EQ(u, u);

        std::stringstream vstr;
        vstr << v;
        EXPECT_EQ(vstr.str(), "[1, 10, 100, 1000]");
    }
}

TEST(vec, testStr)
{
    Vec<long long> v{ 1, 10, 100, 1000 }, w;
    std::stringstream vstr;
    vstr << v << w;
    EXPECT_EQ(vstr.str(), "[1, 10, 100, 1000][]");
}

TEST(vec, testAccess)
{
    Vec<int> v{ 1, 2 };
    int &elem = v[0];
    const int &constElem = v[1];

    EXPECT_EQ(elem, 1);
    EXPECT_EQ(constElem, 2);
    elem = 10;
    EXPECT_EQ(elem, 10);

    //EXPECT_TRUE(v == { 10, 2 });
}

TEST(vec, testCopy)
{
    Vec<int> v{ 1, 3, 5 }, w = v;
    auto u(v);
    auto vv(v);

    EXPECT_EQ(v, w);
    EXPECT_EQ(v, u);
    EXPECT_EQ(v, vv);
    EXPECT_EQ(v, v);

    // make sure than operator== is c`orrect
    for (int i = 0; i < v.ndim(); ++i)
        EXPECT_EQ(v[i], u[i]);
}

TEST(vec, testTypeCast)
{
    Vec<double> v{ 1.0f, 2.0f };
    const char a = 1, b = 10;
    Vec<unsigned short> vushort{ a, b };  // template initializer_list constructor allows this
    v -= vushort;  // template copy constructor allows this
    EXPECT_TRUE(v.almostEqualTo({ 0, -8 }));
}

TEST(vec, testPlus)
{
    Vec<int> v{ 1, 2 }, w{ 3, 4 };
    v += w;
    EXPECT_EQ(v, Vec<int>({ 4, 6 }));

    const auto u = v + w;
    EXPECT_EQ(u, Vec<int>({ 7, 10 }));
    EXPECT_EQ(v, Vec<int>({ 4, 6 }));
    EXPECT_EQ(w, Vec<int>({ 3, 4 }));

    v = v + w;
    EXPECT_EQ(v, Vec<int>({ 7, 10 }));

    v += Vec<int>({ 1, -1 });
    EXPECT_EQ(v, Vec<int>({ 8, 9 }));

    v += {2, 3};  // works because of initializer list constructor
    EXPECT_EQ(v, Vec<int>({ 10, 12 }));

    // udacity quiz questions
    const auto sum = Vec<float>({ 8.218f, -9.341f }) + Vec<float>({ -1.129f, 2.111f });
    EXPECT_TRUE(sum.almostEqualTo({ 7.089f, -7.23f }));
}

TEST(vec, testMinus)
{
    Vec<int> v{ 1, 2 }, w{ 3, 4 };
    v -= w;
    EXPECT_EQ(v, Vec<int>({ -2, -2 }));

    const auto u = v - w;
    EXPECT_EQ(u, Vec<int>({ -5, -6 }));
    EXPECT_EQ(v, Vec<int>({ -2, -2 }));
    EXPECT_EQ(w, Vec<int>({ 3, 4 }));

    // udacity quiz questions
    const auto diff = Vec<float>({ 7.119f, 8.215f }) - Vec<float>({ -8.223f, 0.878f });
    EXPECT_TRUE(diff.almostEqualTo({ 15.342f, 7.337f }));
}

TEST(vec, testMulScalar)
{
    Vec<int> v{ 1, 2 };
    v *= 3;
    EXPECT_EQ(v, Vec<int>({ 3, 6 }));

    const auto w = 500 * v * 1;
    EXPECT_EQ(w, Vec<int>({ 1500, 3000 }));

    const auto u = 0 * v * 0;
    EXPECT_EQ(u, Vec<int>({ 0, 0 }));

    v = v * 1;
    EXPECT_EQ(v, Vec<int>({ 3, 6 }));

    // udacity quiz questions
    const auto mul = 7.41f * Vec<float>({ 1.671f, -1.012f, -0.318f });
    EXPECT_TRUE(mul.almostEqualTo({ 12.3821f, -7.49892f, -2.35638f }));
}