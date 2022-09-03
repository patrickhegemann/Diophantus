#pragma once

#include "Variable.hpp"

#include "diophantus/model/numeric/GmpBigInt.hpp"
#include "numeric/BigInt.hpp"

#include <memory>
#include <ostream>

namespace diophantus::model
{
    template <numeric::BigInt NumT>
    struct Assignment
    {
        const Variable variable;
        const NumT value;
    };

    template <numeric::BigInt NumT>
    std::ostream &operator<<(std::ostream &os, const Assignment<NumT>& assignment)
    {
        os << "x" << assignment.variable << " = " << assignment.value;
        return os;
    }

    template struct Assignment<numeric::GmpBigInt>;
}
