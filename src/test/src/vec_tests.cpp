#include <gtest/gtest.h>

#include <linal/vec.hpp>


using namespace Linal;


TEST(vec, testBasic)
{
    {
        const Veci v;
        EXPECT_EQ(v.ndim(), 0);
    }

    {
        const Vecf v{ 1.0f, 2.0f, 3.0f };
        EXPECT_EQ(v.ndim(), 3);
    }
}

TEST(vec, testEq)
{
    const Vec<long long> v{ 1, 10, 100, 1000 }, w{ 1, 10, 100, 1000 }, u{ 2, 3, 4, 5 };
    EXPECT_EQ(v, w);
    EXPECT_NE(v, u);
    EXPECT_NE(w, u);
    EXPECT_EQ(u, u);

    const Vec<unsigned char> uc{ 2, 3, 4, 5 };
    // testing comparison of vectors with different element type
    EXPECT_EQ(uc, u);
    EXPECT_EQ(u, uc);
    EXPECT_NE(v, uc);

    // same, but with equalTo
    EXPECT_TRUE(v.equalTo(w));
    EXPECT_TRUE(u.equalTo(uc));
    EXPECT_TRUE(uc.equalTo(u));
    EXPECT_FALSE(uc.equalTo(w));

    const Vecd vd{ 2, 3, 4, 5 };
    EXPECT_TRUE(vd.equalTo({ 2, 3, 4, 5 }));  // first Vecd is created from list and then equalTo is called
    EXPECT_TRUE(vd.equalTo(u));  // vectors compared directly, element-by-element, no temporary copies created
    EXPECT_TRUE(vd.almostEqualTo({ 2, 3, 4, 5 }));
    EXPECT_TRUE(vd.almostEqualTo(u));
}

TEST(vec, testStr)
{
    const Vec<long long> v{ 1, 10, 100, 1000 }, w;
    std::stringstream vstr;
    vstr << v << w;
    EXPECT_EQ(vstr.str(), "[1, 10, 100, 1000][]");
}

TEST(vec, testNaN)
{
    const Vecf v{ 1.0f, 2.0f }, w(20000, std::numeric_limits<float>::quiet_NaN());
    const Veci vi{ 0 };
    EXPECT_FALSE(v.isNaN());
    EXPECT_TRUE(w.isNaN());
    EXPECT_FALSE(vi.isNaN());
}

TEST(vec, testZero)
{
    const Veci v{ 0, 0 }, w{ 1, 1 }, u{ -2, 0 };
    const Vecf vf{ 0.0f, 0.0f }, wf{ 1.0f, 0.0f };
    EXPECT_TRUE(v.isZero() && !w.isZero());
    EXPECT_TRUE(vf.isZero() && !wf.isZero());
    EXPECT_FALSE(u.isZero());
}

TEST(vec, testAccess)
{
    Veci v{ 1, 2 };
    int &elem = v[0];
    const int &constElem = v[1];

    EXPECT_EQ(elem, 1);
    EXPECT_EQ(constElem, 2);
    elem = 10;
    EXPECT_EQ(elem, 10);

    EXPECT_TRUE(v.equalTo({ 10, 2 }));
    EXPECT_TRUE(v.equalTo({ short(10), short(2) }));

    const Veci w{ 1, 10 }, u{ 1, 10 };
    const std::vector<short> stdv{-1, -1};
    // use Vec::ptr instead of iterators
    EXPECT_TRUE(std::equal(w.ptr(), w.ptr() + w.ndim(), u.ptr()));
    EXPECT_EQ(std::inner_product(w.ptr(), w.ptr() + w.ndim(), u.ptr(), 0), 101);
    EXPECT_EQ(std::inner_product(w.ptr(), w.ptr() + w.ndim(), stdv.begin(), 0), -11);
    EXPECT_EQ(std::accumulate(u.ptr(), u.ptr() + u.ndim(), 0), 11);
}

TEST(vec, testCopy)
{
    Veci v{ 1, 3, 5 }, w = v;
    auto u(v);
    auto vv(v);

    EXPECT_EQ(v, w);
    EXPECT_EQ(v, u);
    EXPECT_EQ(v, vv);
    EXPECT_EQ(v, v);

    // make sure than operator== is correct
    for (int i = 0; i < v.ndim(); ++i)
        EXPECT_EQ(v[i], u[i]);
}

TEST(vec, testTypeCast)
{
    Vecd v{ 1.0f, 2.0f };
    const char a = 1, b = 10;
    Vec<unsigned short> vushort{ a, b };  // template initializer_list constructor allows this
    v -= vushort;  // template copy constructor allows this
    EXPECT_TRUE(v.almostEqualTo({ 0, -8 }));
}

TEST(vec, testPlus)
{
    Veci v{ 1, 2 }, w{ 3, 4 };
    v += w;
    EXPECT_EQ(v, Veci({ 4, 6 }));

    const auto u = v + w;
    EXPECT_EQ(u, Veci({ 7, 10 }));
    EXPECT_EQ(v, Veci({ 4, 6 }));
    EXPECT_EQ(w, Veci({ 3, 4 }));

    v = v + w;
    EXPECT_EQ(v, Veci({ 7, 10 }));

    v += Veci({ 1, -1 });
    EXPECT_EQ(v, Veci({ 8, 9 }));

    v += {2, 3};  // works because of initializer list constructor
    EXPECT_EQ(v, Veci({ 10, 12 }));

    const Veci vi{ 1, 2 };
    const Vecs vs{ -1, -2 };
    const Vecf vf{ 0.0f, 1.0f };
    const Vecd vd{ 1.0, 0.0 };

    {
        auto vres = vi + vd;
        vres = vres + (vi + vd);
        vres = vres + vres;
        static_assert(std::is_same_v<decltype(vres), Vecd>, "Wrong type of result");
        EXPECT_TRUE(vres.almostEqualTo({ 8.0, 8.0 }));
    }

    {
        const auto vres = vd + vf + vi + vs;
        static_assert(std::is_same_v<decltype(vres), const Vecd>, "Wrong type of result");
        EXPECT_TRUE(vres.almostEqualTo({ 1.0, 1.0 }));
    }

    {
        const auto vres = vs + vi;
        static_assert(std::is_same_v<decltype(vres), const Veci>, "Wrong type of result");
        EXPECT_EQ(vres, Veci({ 0, 0 }));
    }

    // udacity quiz questions
    const auto sum = Vecf({ 8.218f, -9.341f }) + Vecf({ -1.129f, 2.111f });
    EXPECT_TRUE(sum.almostEqualTo({ 7.089f, -7.23f }));
}

TEST(vec, testMinus)
{
    Veci v{ 1, 2 }, w{ 3, 4 };
    v -= w;
    EXPECT_EQ(v, Veci({ -2, -2 }));

    const auto u = v - w;
    EXPECT_EQ(u, Veci({ -5, -6 }));
    EXPECT_EQ(v, Veci({ -2, -2 }));
    EXPECT_EQ(w, Veci({ 3, 4 }));

    const Veci vi{ 1, 2 };
    const Vecs vs{ -1, -2 };
    const Vecf vf{ 0.0f, 1.0f };
    const Vecd vd{ 1.0, 0.0 };

    const auto vres = vd - vf - vi - vs;
    static_assert(std::is_same_v<decltype(vres), const Vecd>, "Wrong type of result");
    EXPECT_TRUE(vres.almostEqualTo({ 1.0, -1.0 }));

    // udacity quiz questions
    const auto diff = Vecf({ 7.119f, 8.215f }) - Vecf({ -8.223f, 0.878f });
    EXPECT_TRUE(diff.almostEqualTo({ 15.342f, 7.337f }));
}

TEST(vec, testMulScalar)
{
    Veci v{ 1, 2 };
    v *= 3;
    EXPECT_EQ(v, Veci({ 3, 6 }));

    const auto w = 500 * v * 1;
    EXPECT_EQ(w, Veci({ 1500, 3000 }));

    const auto u = 0 * v * 0;
    EXPECT_EQ(u, Veci({ 0, 0 }));

    v = v * 1;
    EXPECT_EQ(v, Veci({ 3, 6 }));

    // udacity quiz questions
    const auto mul = 7.41f * Vecf({ 1.671f, -1.012f, -0.318f });
    EXPECT_TRUE(mul.almostEqualTo({ 12.3821f, -7.49892f, -2.35638f }));
}

TEST(vec, testNorm)
{
    Vecd v{ 1, -1, 1 };
    EXPECT_EQ(v.normSquared(), 3);
    EXPECT_EQ(v.norm(), sqrt(3));
    v.normalize();
    EXPECT_TRUE(v.almostEqualTo({ 1 / sqrt(3), -1 / sqrt(3), 1 / sqrt(3) }));

    Vecd v0{ 0, 0, 0 };
    v0.normalize();
    EXPECT_TRUE(v0.isNaN());
    EXPECT_TRUE(v0.normalized().isNaN());

    const Veci vi{ 3, 4 };
    EXPECT_EQ(vi.norm(), 5);
    const auto vin = vi.normalized();
    static_assert(std::is_same_v<decltype(vin), const Vecd>, "Wrong result type");
    EXPECT_TRUE(vin.almostEqualTo({ 3.0 / 5, 4.0 / 5 }));

    // udacity quiz questions
    const Vecf v1{ -0.221f, 7.437f };
    EXPECT_FLOAT_EQ(v1.norm(), 7.4402828f);
    const Vecf v2{ 8.813f, -1.331f, -6.247f };
    EXPECT_FLOAT_EQ(v2.norm(), 10.884187f);
    Vecf v3{ 5.581f, -2.136f };
    v3.normalize();
    EXPECT_TRUE(v3.almostEqualTo({ 0.933935f, -0.357442f }));
    Vecf v4{ 1.996f, 3.108f, -4.554f };
    v4.normalize();
    EXPECT_TRUE(v4.almostEqualTo({ 0.340401f, 0.530044f, -0.776647f }));
}

TEST(vec, testDotProduct)
{
    const Veci v{ 1, 2 }, w{ 3, 4 };
    EXPECT_EQ(v * w, 3 + 8);

    const Vecf a{ 10.0f }, b{ 10.0f, 20.0f };
    EXPECT_FLOAT_EQ(a.dot({ 20.0f }), 200.0f);

    const auto res1 = v * b, res2 = b * v;
    static_assert(std::is_same_v<decltype(res1), const float>, "Incorrect result of inner product");
    static_assert(std::is_same_v<decltype(res2), const float>, "Incorrect result of inner product");
    EXPECT_FLOAT_EQ(res1, 50);
    EXPECT_FLOAT_EQ(res2, 50);

    // udacity quiz questions
    const Vecd v1{ 7.887, 4.138 }, w1{ -8.802, 6.776 };
    EXPECT_DOUBLE_EQ(v1 * w1, -41.382286);

    const Vecd v2{ -5.955, -4.904, -1.874 }, w2{ -4.496, -8.755, 7.103 };
    EXPECT_DOUBLE_EQ(v2.dot(w2), 56.397178);
    EXPECT_DOUBLE_EQ(v2.dot(w2), v2 * w2);
}

TEST(vec, testAngle)
{
    const Vecf v{ 1.0f, 2.0f, 3.0f }, w{ 0.0f, 0.0f, 0.0f };
    EXPECT_TRUE(isnan(v.angleToRad(w)));
    EXPECT_TRUE(isnan(w.angleToRad(v)));
    EXPECT_TRUE(isnan(w.angleToDegrees(v)));

    const Veci vi{ 10, 0 }, wi{ 0, 10 }, ui{ 10, 10 };
    EXPECT_DOUBLE_EQ(vi.angleToDegrees(wi), 90);
    EXPECT_DOUBLE_EQ(vi.angleToDegrees(ui), 45);
    EXPECT_DOUBLE_EQ(ui.angleToDegrees(ui), 0);
    EXPECT_DOUBLE_EQ((-1 * vi).angleToDegrees(ui), 135);
    EXPECT_DOUBLE_EQ((-1 * vi).angleToRad(ui), 3.0 / 4.0 * M_PI);
    
    const Vecd vd{ 11, 11 };
    EXPECT_DOUBLE_EQ(ui.angleToRad(vd), 0);
    const Vecf vf{ -12.0f, 12.0f };
    const auto angle1 = ui.angleToDegrees(vf), angle2 = vf.angleToDegrees(ui);
    static_assert(std::is_same_v<decltype(angle1), const float>, "Angle type is incorrect");
    static_assert(std::is_same_v<decltype(angle2), const float>, "Angle type is incorrect");
    EXPECT_FLOAT_EQ(ui.angleToDegrees(vf), 90);
    EXPECT_FLOAT_EQ(vf.angleToDegrees(ui), 90);

    // udacity quiz questions
    const Vecf v1{ 3.183f, -7.627f }, w1{ -2.668f, 5.319f };
    EXPECT_FLOAT_EQ(v1.angleToRad(w1), 3.0720272f);
    const Vecf v2{ 7.35f, 0.221f, 5.188f }, w2{ 2.751f, 8.259f, 3.985f };
    EXPECT_FLOAT_EQ(v2.angleToDegrees(w2), 60.2758f);
}

TEST(vec, testParallelOrthogonal)
{
    const Vecf v{ 1.0f, 2.0f, 3.0f }, w{ 3.0f, 6.0f, 9.0f }, z{ 0.0f, 0.0f, 0.0f };
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

    const Veci vi{ 1, 2 }, wi{ 2, 4 }, vio{ -2, 1 }, zi{ 0, 0 };
    EXPECT_TRUE(vi.isParallelTo(wi));

    EXPECT_TRUE(vi.isOrthogonalTo(vio));
    EXPECT_TRUE(vio.isOrthogonalTo(vi));
    EXPECT_TRUE(vio.isOrthogonalTo(wi));

    EXPECT_TRUE(vi.isOrthogonalTo(zi));
    EXPECT_TRUE(zi.isOrthogonalTo(vi));
    EXPECT_TRUE(zi.isOrthogonalTo(zi));

    EXPECT_FALSE(vi.isParallelTo(vio));
    EXPECT_FALSE(vio.isParallelTo(vi));

    const Vecd vd{ 10, 20 }, vdo{ -20, 10 }, zd{ 0, 0 };
    EXPECT_TRUE(vd.isOrthogonalTo(vdo));
    EXPECT_TRUE(vdo.isOrthogonalTo(vd));
    EXPECT_TRUE(vdo.isOrthogonalTo(zd));
    EXPECT_TRUE(zd.isOrthogonalTo(zd));

    EXPECT_TRUE(zd.isOrthogonalTo(zi));  // conversion from int to double - okay

    // udacity quiz questions
    const Vecf v1 = { -7.579f, -7.88f }, w1 = { 22.737f, 23.64f };  // trying different type of initialization
    EXPECT_TRUE(v1.isParallelTo(w1));
    EXPECT_FALSE(v1.isOrthogonalTo(w1));
    const Vecf v2{ -2.029f, 9.97f, 4.172f }, w2{ -9.231f, -6.639f, -7.245f };
    EXPECT_FALSE(v2.isParallelTo(w2));
    EXPECT_FALSE(v2.isOrthogonalTo(w2));
    const Vecf v3{ -2.328f, -7.284f, -1.214f }, w3{ -1.821f, 1.072f, -2.94f };
    EXPECT_FALSE(v3.isParallelTo(w3));
    EXPECT_TRUE(v3.isOrthogonalTo(w3));
    const Vecf v4{ 2.118f, 4.827f }, w4{ 0.0f, 0.0f };
    EXPECT_TRUE(v4.isParallelTo(w4));
    EXPECT_TRUE(v4.isOrthogonalTo(w4));
}

TEST(vec, testProjection)
{
    {
        const Veci v{ 10, 0 }, w{ 5, 5 };
        const auto v1 = w.componentParallelTo(v), v2 = w.componentOrthogonalTo(v);
        EXPECT_TRUE(v1.almostEqualTo({ 5, 0 }));
        EXPECT_TRUE(v2.almostEqualTo({ 0, 5 }));
    }

    // vectors of different types
    {
        const Veci v{ 2, 3 };
        const Vecf w{ 0.0f, -10.0f };
        const auto v1 = v.componentParallelTo(w), v2 = v.componentOrthogonalTo(w);
        static_assert(std::is_same_v<decltype(v1), const Vecf>, "Wrong component type");
        EXPECT_TRUE(v1.almostEqualTo({ 0.0f, 3.0f }));
        EXPECT_TRUE(v2.almostEqualTo({ 2.0f, 0.0f }));
    }

    // zero vectors
    {
        const Veci v{ 1, 2 }, w{ 0, 0 };
        const auto v1 = v.componentParallelTo(w), v2 = v.componentOrthogonalTo(w);
        EXPECT_TRUE(v1.isNaN() && v2.isNaN());
    }

    // udacity quiz questions
    {
        const Vecd v1{ 3.039, 1.879 }, b1{ 0.825, 2.036 };
        const auto par1 = v1.componentParallelTo(b1);
        EXPECT_TRUE(par1.almostEqualTo({ 1.08261, 2.67174 }));

        const Vecd v2{ -9.88, -3.264, -8.159 }, b2{ -2.155, -9.353, -9.473 };
        const auto ort2 = v2.componentOrthogonalTo(b2);
        EXPECT_TRUE(ort2.almostEqualTo({ -8.35008, 3.37606, -1.43375 }));

        const Vecd v3{ 3.009, -6.172, 3.692, -2.51 }, b3{ 6.404, -9.144, 2.759, 8.718 };
        const auto par3 = v3.componentParallelTo(b3), ort3 = v3.componentOrthogonalTo(b3);
        EXPECT_TRUE(par3.almostEqualTo({ 1.96852, -2.81076, 0.848085, 2.67981 }));
        EXPECT_TRUE(ort3.almostEqualTo({ 1.04048, -3.36124, 2.84392, -5.18981 }));
    }
}

TEST(vec, testCross)
{
    const Veci v{ 1, 0 }, w{ 0, 1 };
    const auto vxw = v.cross(w);
    static_assert(std::is_same_v<decltype(vxw), const Veci>, "Incorrect type of cross product");
    EXPECT_TRUE(vxw.equalTo({ 0, 0, 1 }));
    EXPECT_TRUE(w.cross(v).equalTo({ 0, 0, -1 }));
    EXPECT_TRUE(w.cross(w).isZero());

    EXPECT_EQ(parallelogramArea(w, w), 0);
    EXPECT_DOUBLE_EQ(triangleArea(w, w), 0.0);

    EXPECT_EQ(parallelogramArea(v, w), 1);
    EXPECT_EQ(triangleArea(v, w), 0.5);

    // udacity quiz questions
    EXPECT_TRUE((Vecd{ 8.462, 7.893, -8.187 }).cross({ 6.984, -5.975, 4.778 }).almostEqualTo({ -11.204571, -97.609444, -105.685162 }));
    EXPECT_FLOAT_EQ(float(parallelogramArea(Vecd{ -8.987, -9.838, 5.031 }, Vecd{ -4.268, -1.861, -8.866 })), 142.122221f);
    EXPECT_FLOAT_EQ(float(triangleArea(Vecd{ 1.5, 9.547, 3.691 }, Vecd{ -6.007, 0.124, 5.772 })), 42.5649374f);
}