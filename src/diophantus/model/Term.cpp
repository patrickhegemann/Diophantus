#include "Term.hpp"

#include <diophantus/model/Variable.hpp>

#include <utility>

namespace diophantus::model
{
    template <numeric::BigInt NumT>
    Term<NumT>::Term(const NumT& coefficient, const Variable variable) :
        coefficient(std::move(coefficient)),
        variable(std::move(variable))
    {}

    template <numeric::BigInt NumT>
    Term<NumT>::Term(const long coefficient, const Variable variable) :
        coefficient(NumT(coefficient)),
        variable(std::move(variable))
    {}

    template <numeric::BigInt NumT>
    const NumT& Term<NumT>::getCoefficient() const
    {
        return coefficient;
    }

    template <numeric::BigInt NumT>
    Variable Term<NumT>::getVariable() const
    {
        return variable;
    }

    template <numeric::BigInt NumT>
    void Term<NumT>::divideCoefficientBy(const NumT& divisor)
    {
        coefficient /= divisor;
    }

    template <numeric::BigInt NumT>
    void Term<NumT>::multiplyCoefficientBy(const NumT& factor)
    {
        coefficient *= factor;
    }

    template <numeric::BigInt NumT>
    void Term<NumT>::coefficientMod(const NumT& modulus)
    {
        coefficient = NumT::symMod(coefficient, modulus);
    }

    template <numeric::BigInt NumT>
    void Term<NumT>::setCoefficientToZero()
    {
        coefficient = NumT(0);
    }

    template class Term<numeric::GmpBigInt>;
}
