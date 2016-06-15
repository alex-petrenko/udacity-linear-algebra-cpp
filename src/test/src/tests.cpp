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

TEST(vec, testNorm)
{
    Vec<double> v{ 1, -1, 1 };
    EXPECT_EQ(v.normSquared(), 3);
    EXPECT_EQ(v.norm(), sqrt(3));
    v.normalize();
    EXPECT_TRUE(v.almostEqualTo({ 1 / sqrt(3), -1 / sqrt(3), 1 / sqrt(3) }));

    Vec<double> v0{ 0, 0, 0 };
    v0.normalize();
    EXPECT_TRUE(v0.equalTo({ 0, 0, 0 }));

    // udacity quiz questions
    const Vec<float> v1{ -0.221f, 7.437f };
    EXPECT_FLOAT_EQ(v1.norm(), 7.4402828f);
    const Vec<float> v2{ 8.813f, -1.331f, -6.247f };
    EXPECT_FLOAT_EQ(v2.norm(), 10.884187f);
    Vec<float> v3{ 5.581f, -2.136f };
    v3.normalize();
    EXPECT_TRUE(v3.almostEqualTo({ 0.933935f, -0.357442f }));
    Vec<float> v4{ 1.996f, 3.108f, -4.554f };
    v4.normalize();
    EXPECT_TRUE(v4.almostEqualTo({ 0.340401f, 0.530044f, -0.776647f }));
}

TEST(vec, testDotProduct)
{
    Vec<int> v{ 1, 2 }, w{ 3, 4 };
    EXPECT_EQ(v * w, 3 + 8);

    Vec<float> a{ 10.0f };
    EXPECT_FLOAT_EQ(a.dot({ 20.0f }), 200.0f);

    // udacity quiz questions
    const Vec<double> v1{ 7.887, 4.138 }, w1{ -8.802, 6.776 };
    EXPECT_DOUBLE_EQ(v1 * w1, -41.382286);

    const Vec<double> v2{ -5.955, -4.904, -1.874 }, w2{ -4.496, -8.755, 7.103 };
    EXPECT_DOUBLE_EQ(v2.dot(w2), 56.397178);
    EXPECT_DOUBLE_EQ(v2.dot(w2), v2 * w2);
}

TEST(vec, testAngle)
{
    const Vec<float> v{ 1.0f, 2.0f, 3.0f }, w{ 0.0f, 0.0f, 0.0f };
    EXPECT_TRUE(isnan(v.angleToRad(w)));
    EXPECT_TRUE(isnan(w.angleToRad(v)));
    EXPECT_TRUE(isnan(w.angleToDegrees(v)));

    // udacity quiz questions
    const Vec<float> v1{ 3.183f, -7.627f }, w1{ -2.668f, 5.319f };
    EXPECT_FLOAT_EQ(v1.angleToRad(w1), 3.0720272f);
    const Vec<float> v2{ 7.35f, 0.221f, 5.188f }, w2{ 2.751f, 8.259f, 3.985f };
    EXPECT_FLOAT_EQ(v2.angleToDegrees(w2), 60.2758f);
}

TEST(vec, testParallelOrthogonal)
{
    const Vec<float> v{ 1.0f, 2.0f, 3.0f }, w{ 3.0f, 6.0f, 9.0f }, z{ 0.0f, 0.0f, 0.0f };
    EXPECT_TRUE(v.isParallelTo(w));
    EXPECT_TRUE(v.isParallelTo(v));
    EXPECT_TRUE(w.isParallelTo(v));
    EXPECT_TRUE(w.isParallelTo(w));

    EXPECT_TRUE(v.isParallelTo(z));
    EXPECT_TRUE(z.isParallelTo(v));
    EXPECT_TRUE(z.isParallelTo(z));
    EXPECT_TRUE(z.isOrthogonalTo(z));

    EXPECT_FALSE(v.isOrthogonalTo(w));
    EXPECT_FALSE(w.isOrthogonalTo(v));

    const Vec<int> vi{ 1, 2 }, wi{ 2, 4 }, vio{ -2, 1 }, zi{ 0, 0 };
    EXPECT_TRUE(vi.isParallelTo(wi));

    EXPECT_TRUE(vi.isOrthogonalTo(vio));
    EXPECT_TRUE(vio.isOrthogonalTo(vi));
    EXPECT_TRUE(vio.isOrthogonalTo(wi));

    EXPECT_TRUE(vi.isOrthogonalTo(zi));
    EXPECT_TRUE(zi.isOrthogonalTo(vi));
    EXPECT_TRUE(zi.isOrthogonalTo(zi));

    EXPECT_FALSE(vi.isParallelTo(vio));
    EXPECT_FALSE(vio.isParallelTo(vi));

    const Vec<double> vd{ 10, 20 }, vdo{ -20, 10 }, zd{ 0, 0 };
    EXPECT_TRUE(vd.isOrthogonalTo(vdo));
    EXPECT_TRUE(vdo.isOrthogonalTo(vd));
    EXPECT_TRUE(vdo.isOrthogonalTo(zd));
    EXPECT_TRUE(zd.isOrthogonalTo(zd));

    EXPECT_TRUE(zd.isOrthogonalTo(zi));  // conversion from int to double - okay

    // udacity quiz questions
    const Vec<float> v1 = { -7.579f, -7.88f }, w1 = { 22.737f, 23.64f };  // trying different type of initialization
    EXPECT_TRUE(v1.isParallelTo(w1));
    EXPECT_FALSE(v1.isOrthogonalTo(w1));
    const Vec<float> v2{ -2.029f, 9.97f, 4.172f }, w2{ -9.231f, -6.639f, -7.245f };
    EXPECT_FALSE(v2.isParallelTo(w2));
    EXPECT_FALSE(v2.isOrthogonalTo(w2));
    const Vec<float> v3{ -2.328f, -7.284f, -1.214f }, w3{ -1.821f, 1.072f, -2.94f };
    EXPECT_FALSE(v3.isParallelTo(w3));
    EXPECT_TRUE(v3.isOrthogonalTo(w3));
    const Vec<float> v4{ 2.118f, 4.827f }, w4{ 0.0f, 0.0f };
    EXPECT_TRUE(v4.isParallelTo(w4));
    EXPECT_TRUE(v4.isOrthogonalTo(w4));
}