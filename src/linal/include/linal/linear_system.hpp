#pragma once

#include <linal/linear_equation.hpp>


namespace Linal
{

enum class SolutionType
{
    UNKNOWN,
    UNIQUE,
    PARAMETRIZATION,
    NO_SOLUTIONS,
};

/// Represents solution as parameterization:
/// solutionSet = basepoint + v1 * t + v2 * s + ...
template <typename T>
class LinearSystemSolution
{
public:
    explicit LinearSystemSolution(SolutionType type, Vec<T> basepoint = Vec<T>{}, std::vector<Vec<T>> parametrization = std::vector<Vec<T>>{})
        : type{ type }
        , basepoint(std::move(basepoint))
        , parametrization(std::move(parametrization))
    {
    }

public:
    friend std::ostream & operator<<(std::ostream &stream, const LinearSystemSolution<T> &s)
    {
        stream << "[Type: " << int(s.type);
        if (s.type == SolutionType::UNIQUE || s.type == SolutionType::PARAMETRIZATION)
        {
            stream << " Solution: [" << s.basepoint;
            for (int i = 0; i < int(s.parametrization.size()); ++i)
                if (!s.parametrization[i].isZero())
                    stream << " + " << s.parametrization[i] << "*t" << (i + 1);
            stream << "]";
        }
        stream << "]";
        return stream;
    }

public:
    Vec<T> basepoint;
    std::vector<Vec<T>> parametrization;
    SolutionType type = SolutionType::UNKNOWN;
};

template <typename T>
class LinearSystem
{
public:
    LinearSystem(std::initializer_list<LinearEquation<T>> l)
        : eqs(l.size())
    {
        /* Currently there's no way to move from initializer_list, so we're actually copying.
        * http://stackoverflow.com/questions/8193102/initializer-list-and-move-semantics
        */
        std::move(l.begin(), l.end(), eqs.begin());
    }

    const LinearEquation<T> & operator[](int idx) const
    {
        return eqs[idx];
    }

    LinearEquation<T> & operator[](int idx)
    {
        return eqs[idx];
    }

    void swapRows(int i, int j)
    {
        assert(i >= 0 && i < int(eqs.size()) && j >= 0 && j < int(eqs.size()) && i != j);
        assert(eqs[i].ndim() == eqs[j].ndim());
        swap(eqs[i], eqs[j]);  // very cheap operation
    }

    /// Turns linear system into triangular form in-place.
    /// Assumes all rows have the same number of variables.
    void computeTriangularForm()
    {
        // eliminating i-th variable from rows i+1 and below
        for (int i = 0; i < eqs.front().ndim(); ++i)
        {
            // trying to find row with non-zero coefficient at i-th variable
            int rowWithIthVar = -1;
            for (int j = 0; j < int(eqs.size()); ++j)
            {
                const int idx = eqs[j].firstNonZeroIndex();

                if (idx == -1)
                    continue;

                // rows below i-1 can't have i-1 as leading variable
                assert(i == 0 || j < i || idx >= i);

                if (idx == i)
                {
                    rowWithIthVar = j;  // found row with leading i-th variable
                    break;
                }
            }

            if (rowWithIthVar == -1)
            {
                // i-th variable already eliminated from the system
                continue;
            }

            // swap row with "current" row
            if (rowWithIthVar > i)
                swapRows(i, rowWithIthVar);

            // subtract found row from all rows below
            for (int j = i + 1; j < int(eqs.size()); ++j)
            {
                const auto coeff = (-1.0f / eqs[i][i]) * eqs[j][i];
                eqs[j].add(eqs[i], coeff);
            }
        }
    }

    /// Computes reduced row-echelon form in-place.
    void computeRREF()
    {
        computeTriangularForm();

        // iterate from last to first row, eliminating leading variable from rows above
        for (int i = int(eqs.size()) - 1; i >= 0; --i)
        {
            const int idx = eqs[i].firstNonZeroIndex();
            if (idx == -1)  // no variables (0 = 0 or 0 = k)
                continue;

            // make sure coefficient at leading variable is one
            eqs[i] *= 1.0f / eqs[i][idx];

            // subtract row from all rows above
            for (int j = i - 1; j >= 0; --j)
                eqs[j].add(eqs[i], -eqs[j][idx]);
        }
    }

    auto computeSolution()
    {
        computeRREF();

        const int ndim = eqs.front().ndim();
        Vec<T> basepoint(ndim, T(0));
        std::vector<Vec<T>> parametrization(ndim, Vec<T>(ndim, T(0)));
        bool uniqueSolution = true;
        for (const auto &eq : eqs)
        {
            const int idx = eq.firstNonZeroIndex();
            if (idx == -1)
            {
                // row 0x + 0y + 0z = ?
                if (std::abs(eq.constTerm()) > eq.epsilon())  // contradiction
                    return LinearSystemSolution<T>(SolutionType::NO_SOLUTIONS);

                continue;
            }

            basepoint[idx] = eq.constTerm();
            for (int i = idx + 1; i < ndim; ++i)
                if (std::abs(eq[i]) > eq.epsilon())
                {
                    uniqueSolution = false;

                    parametrization[i][idx] = -eq[i];  // idx variable depends on i-th variable (e.g. x + 2y = 10  -->  x = 10 - 2y)

                    assert(parametrization[i][i] == T(0) || parametrization[i][i] == T(1));
                    parametrization[i][i] = T(1);  // i-th variable is a free variable (e.g. z = z)
                }
        }

        const SolutionType type = uniqueSolution ? SolutionType::UNIQUE : SolutionType::PARAMETRIZATION;
        return LinearSystemSolution<T>(type, std::move(basepoint), std::move(parametrization));
    }

public:
    friend std::ostream & operator<<(std::ostream &stream, const LinearSystem<T> &s)
    {
        for (const auto &eq : s.eqs)
            stream << eq << std::endl;
        return stream;
    }

private:
    std::vector<LinearEquation<T>> eqs;
};

}