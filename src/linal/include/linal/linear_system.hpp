#pragma once

#include <linal/linear_equation.hpp>


namespace Linal
{

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
        assert(i >= 0 && i < int(eqs.size()) && j >= 0 && j < int(eqs.size()));
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
            if (rowWithIthVar != i)
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