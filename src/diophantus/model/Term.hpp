#pragma once

#include "numeric/GmpBigInt.hpp"
#include "numeric/BigInt.hpp"

#include <memory>
#include <set>

namespace diophantus::model
{
    struct Variable;

    template <numeric::BigInt NumT>
    class Term
    {
        public:
            Term<NumT>(const NumT& coefficient, const std::shared_ptr<Variable> variable);

            /**
             * Constructor which converts the long coefficient to a NumT.
             */
            Term<NumT>(const long coefficient, const std::shared_ptr<Variable> variable);

            // Getters
            const NumT& getCoefficient() const;
            const std::shared_ptr<Variable> getVariable() const;

            /**
             * Divides the coefficient by a divisor.
             * @param divisor
             */
            void divideCoefficientBy(const NumT& divisor);

            /**
             * Multiply the coefficient by a factor.
             * @param factor
             */
            void multiplyCoefficientBy(const NumT& factor);

            /**
             * Takes the coefficient mod another number.
             */
            void coefficientMod(const NumT& modulus);

            /**
             * Sets the coefficient to zero.
             */
            void setCoefficientToZero();

            friend std::ostream &operator<<(std::ostream &os, const Term<NumT>& term)
            {
                os << "(" << term.coefficient << ")*x[" << term.variable->variableNumber << "]";
                return os;
            }

        private:
            NumT coefficient;
            std::shared_ptr<Variable> variable;
    };
}
