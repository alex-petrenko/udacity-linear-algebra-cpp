#include <gtest/gtest.h>

#include <linal/line2d.hpp>
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
    EXPECT_TRUE(csys[0].equalTo({ { 1, 1, 1 }, 1 }));
    LinearSystem<int> sys{ csys };
    sys[1] = LinearEquation<int>{ {1, 2, 3 }, 4 };
    EXPECT_TRUE(sys[1].equalTo({ { 1, 2, 3 }, 4 }));
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

    {
        const Plane3d p1{ { 1, 1, 1 }, 1 }, p2{ { 1, 1, 1 }, 2 };
        LinearSystem<double> s{ p1, p2 };
        s.computeTriangularForm();
        EXPECT_TRUE(s[0] == p1 && s[1].equalTo({ {0, 0, 0}, 1 }));
    }

    {
        const Plane3d p1{ {1, 1, 1}, 1 }, p2{ {0, 1, 0}, 2 }, p3{ {1, 1, -1}, 3 }, p4{ {1, 0, -2}, 2 };
        LinearSystem<double> s{ p1, p2, p3, p4 };
        s.computeTriangularForm();
        EXPECT_TRUE(s[0] == p1 && s[1] == p2 && s[2].equalTo({ {0, 0, -2}, 2 }) && s[3].equalTo({ { 0, 0, 0 }, 0 }));
    }

    {
        const Plane3d p1{ {0, 1, 1}, 1 }, p2{ {1, -1, 1}, 2 }, p3{ {1, 2, -5}, 3 };
        LinearSystem<double> s{ p1, p2, p3 };
        s.computeTriangularForm();
        EXPECT_TRUE(s[0].equalTo({ {1, -1, 1}, 2 }) && s[1].equalTo({ {0, 1, 1}, 1 }) && s[2].equalTo({ {0, 0, -9}, -2 }));
        s.computeTriangularForm();
        EXPECT_TRUE(s[0].equalTo({ { 1, -1, 1 }, 2 }) && s[1].equalTo({ { 0, 1, 1 }, 1 }) && s[2].equalTo({ { 0, 0, -9 }, -2 }));
    }
}

TEST(linearSystem, testRREF)
{
    {
        const Plane3d p1{ {1, 1, 1}, 1 }, p2{ {0, 1, 1}, 2 };
        LinearSystem<double> s{ p1, p2 };
        s.computeRREF();
        EXPECT_TRUE(s[0].equalTo({ {1, 0, 0}, -1 }) && s[1] == p2);
    }

    {
        const Plane3d p1{ {1, 1, 1}, 1 }, p2{ {1, 1, 1}, 2 };
        LinearSystem<double> s{ p1, p2 };
        s.computeRREF();
        EXPECT_TRUE(s[0] == p1 && s[1].equalTo({ {0, 0, 0}, 1 }));
    }

    {
        const Plane3d p1{ {1, 1, 1}, 1 }, p2{ {0, 1, 0}, 2 }, p3{ {1, 1, -1}, 3 }, p4{ {1, 0, -2}, 2 };
        LinearSystem<double> s{ p1, p2, p3, p4 };
        s.computeRREF();
        s[2].equalTo({ { 0, 0, 1 }, -1 });
        EXPECT_TRUE(s[0].equalTo({ {1, 0, 0}, 0 }) && s[1] == p2 && s[2].almostEqualTo({ {0, 0, 1}, -1 }) && s[3].equalTo({ {0, 0, 0}, 0 }));
    }

    {
        const Plane3d p1{ {0, 1, 1}, 1 }, p2{ {1, -1, 1}, 2 }, p3{ {1, 2, -5}, 3 };
        LinearSystem<double> s{ p1, p2, p3 };
        s.computeRREF();
        EXPECT_TRUE(s[0].equalTo({ {1, 0, 0}, 23.0 / 9.0 }));
        EXPECT_TRUE(s[1].equalTo({ {0, 1, 0}, 7.0 / 9.0 }));
        EXPECT_TRUE(s[2].almostEqualTo({ {0, 0, 1}, 2.0 / 9.0 }));
    }
}

TEST(linearSystem, testSolution)
{
    {
        const Line2f l1{ {1.0f, 1.0f}, 1.0f }, l2{ {-1.0f, 1.0f}, 0.0f };
        LinearSystem<float> s{ l1, l2 };
        const auto solution = s.computeSolution();
        EXPECT_TRUE(solution.values.count(0) && solution.values.at(0) == 0.5f);
        EXPECT_TRUE(solution.values.count(1) && solution.values.at(1) == 0.5f);
    }

    {
        const Plane3d p1{ {1, 1, 1}, 1 }, p2{ {1, 0, 10}, 100 };
        LinearSystem<double> s{ p1, p2 };
        const auto solution = s.computeSolution();
        EXPECT_TRUE(solution.type == SolutionType::PARAMETRIZATION);
    }

    // udacity quiz questions
    {
        const Plane3d p1{ {5.862, 1.178, -10.366}, -8.15 }, p2{ {-2.931, -0.589, 5.183}, -4.075 };
        LinearSystem<double> s{ p1, p2 };
        const auto solution = s.computeSolution();
        std::cout << solution << std::endl;
    }

    {
        const Plane3d p1{ {8.631, 5.112, -1.816}, -5.113 }, p2{ {4.315, 11.132, -5.27}, -6.775 }, p3{ {-2.158, 3.01, -1.727}, -0.831 };
        LinearSystem<double> s{ p1, p2 };
        const auto solution = s.computeSolution();
        std::cout << solution << std::endl;
    }

    {
        const Plane3d p1{ {5.262, 2.739, -9.878}, -3.441 }, p2{ {5.111, 6.358, 7.638}, -2.152 }, p3{ {2.016, -9.924, -1.367}, -9.278 }, p4{ {2.167, -13.543, -18.883}, -10.567 };
        LinearSystem<double> s{ p1, p2, p3, p4 };
        const auto solution = s.computeSolution();
        EXPECT_EQ(solution.type, SolutionType::UNIQUE);
        const auto &v = solution.values;
        const Vecd res{ v.at(0), v.at(1), v.at(2) }, expected{ -1.1772, 0.707151, -0.0826636 };
        EXPECT_TRUE(res.almostEqualTo(expected));
    }
}
