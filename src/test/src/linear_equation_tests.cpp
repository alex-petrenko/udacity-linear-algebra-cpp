#include <gtest/gtest.h>

#include <linal/linear_equation.hpp>


using namespace Linal;


/// This test attempts to verify that no matter how we create a linear equation object, no more than one vector
/// copy is performed. See LinearEquation constructor for an explanation.
TEST(linearEquation, testCtor)
{
    {
        // temporary Vec is constructed from initializer_list and then move-ctor moves temporary to eq.norm (MOVE)
        const LinearEquation<float> eq{ { 1.0f, 2.0f, 3.0f}, 4.0f };
        EXPECT_EQ(eq.normVector(), Vecf({ 1.0f, 2.0f, 3.0f }));
        EXPECT_EQ(eq.constTerm(), 4.0f);
    }

    {
        Veci v{ 1, 2 };
        // 'v' is copied into ctor argument and then argument is moved to eq.norm (COPY+MOVE)
        const LinearEquation<int> eq1{ v, 10 };
        EXPECT_TRUE(eq1.normVector() == v && eq1.constTerm() == 10);
        // Pass rvalue. 'v' is moved to ctor argument and then argument is moved to eq.norm (MOVE+MOVE)
        const LinearEquation<int> eq2{ std::move(v), 10 };
        EXPECT_TRUE(eq1.normVector().equalTo({ 1, 2 }) && eq1.constTerm() == 10);

        // v was swapped with default-created Vec, so it is just empty now
        EXPECT_TRUE(v.ndim() == 0 && v.ptr() == nullptr);
    }

    {
        const Veci v{ 1, 2 };
        // Since 'v' is const it cannot be moved, instead copy-ctor is called, and then temporary is moved to eq.norm (COPY+MOVE)
        const LinearEquation<int> eq{ std::move(v), 10 };
        EXPECT_TRUE(eq.normVector() == v && eq.constTerm() == 10);
        EXPECT_TRUE(v.equalTo({ 1, 2 }));
    }
}
