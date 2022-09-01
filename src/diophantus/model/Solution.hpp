#pragma once

#include <diophantus/model/numeric/BigInt.hpp>
#include <diophantus/model/Assignment.hpp>

#include <vector>


namespace diophantus::model
{
    template <numeric::BigInt NumT>
    struct Assignment;

    template <numeric::BigInt NumT>
    struct Solution
    {
        public:
            std::vector<Assignment<NumT>> assignments;
    };

    template <numeric::BigInt NumT>
    std::ostream &operator<<(std::ostream &os, const Solution<NumT>& solution)
    {
        for (const auto& a : solution.assignments)
        {
            os << a << std::endl;
        }
        return os;
    }
}
