#pragma once

#include "Sum.hpp"
#include "Variable.hpp"

#include "diophantus/model/Assignment.hpp"
#include "numeric/GmpBigInt.hpp"
#include "numeric/BigInt.hpp"
#include <memory>
#include <optional>

namespace diophantus::model
{
    template <numeric::BigInt NumT>
    class DeducedEquation
    {
        public:
            DeducedEquation(const Variable variable,
                            const Sum<NumT>& rightSideTerms,
                            const NumT& rightSideConstant) :
                variable(variable),
                rightSideTerms(std::move(rightSideTerms)),
                rightSideConstant(std::move(rightSideConstant))
            {}

            Variable getVariable() const
            {
                return variable;
            }

            const Sum<NumT>& getRightSideSum() const
            {
                return rightSideTerms;
            }

            const NumT& getRightSideConstant() const
            {
                return rightSideConstant;
            }

            /**
             * Adds a term to the right side of the deduced equation.
             * @param term
             */
            void addTerm(const Term<NumT>& term)
            {
                rightSideTerms.addTerm(term);
            }

            /**
             * Takes all coefficients of terms on the right side modulo a number.
             * @param modulus
             */
            void coefficientsModulo(const NumT& modulus)
            {
                rightSideTerms.coefficientsModulo(modulus);
            }

            /**
             * Sets the constant on the right side of the equation.
             * @param constant
             */
            void setRightSideConstant(const NumT& constant)
            {
                rightSideConstant = std::move(constant);
            }

            /**
             * Substitutes a variable on the right side of the equation with a given value.
             * @param assignment
             *      Assignment specifying the variable and its value.
             */
            void substitute(const Assignment<NumT>& assignment)
            {
                std::optional<NumT> coefficient = rightSideTerms.setCoefficientOfVariableToZero(assignment.variable);
                if (coefficient != std::nullopt)
                {
                    rightSideConstant += coefficient.value() * assignment.value;
                }
            }

            friend std::ostream& operator<<(std::ostream& os, const DeducedEquation<NumT>& eq)
            {
                os << "x[" << eq.variable << "] = "
                   << eq.rightSideTerms << " + " << eq.rightSideConstant;
                return os;
            }

        private:
            const Variable variable;
            Sum<NumT> rightSideTerms;
            NumT rightSideConstant;
    };
}
