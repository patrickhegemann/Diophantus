#pragma once

#include "Assignment.hpp"
#include "DeducedEquation.hpp"
#include "SimplificationResult.hpp"
#include "Sum.hpp"
#include "Term.hpp"
#include "Variable.hpp"

#include "numeric/BigInt.hpp"
#include "numeric/GmpBigInt.hpp"

#include <memory>
#include <ostream>
#include <vector>

namespace diophantus::model
{
    template <numeric::BigInt NumT>
    class Equation
    {
        public:
            /**
             * @param leftSide
             *      The sum expression on the left side of the equation
             * @param rightSide
             *      The constant on the right side of the equation
             */
            Equation(const Sum<NumT>& leftSide, const NumT& rightSide);

            Equation(const Sum<NumT>& leftSide, const long rightSide);

            /**
             * Simplify the equation by dividing both sides by the GCD g of all coefficients.
             * @return false if equation has no solution, i.e. g doesn't divide the right side.
             */
            SimplificationResult simplify();

            /**
             * Invert the equation by multiplying both sides with -1
             */
            void invert();

            /**
             * Determines the term with the lowest coefficient in the equation.
             * 
             * @return The term containing the lowest coefficient
             */
            const Term<NumT>& getLowestCoefficientTerm() const;

            /**
             * Determines the term with the highest coefficient in the equation.
             * 
             * @return The term containing the highest coefficient
             */
            const Term<NumT>& getHighestCoefficientTerm() const;
            
            /**
             * Solve the equation for a given variable as if that variable's coefficient was 1.
             * @param variable
             *      The term containing the variable to solve the equation for.
             * @param doNormalInversion
             *      If true, terms that are moved from one side of the equation to the other,
             *      will switch their sign. If false, change of sign is ignored.
             * @returns a deduced equation  TODO:
             */
            DeducedEquation<NumT> solveFor(const Term<NumT>& term, bool doNormalInversion = true);

            /**
             * Solve the equation for a given variable and introduce a new variable, in order to
             * lower the coefficients of all terms.
             * @param term
             *      The term containing the variable to solve the equation for
             * @param newVariable
             *      The new variable to introduce
             * @returns a deduced equation relating the old and new variable to each other.
             */
            DeducedEquation<NumT> eliminate(const Term<NumT>& term,
                                            const Variable newVariable);

            /**
             * Substitute a variable in the equation by an expression (sum).
             * @param deducedEquation
             *      The equation to use for substitution.
             */
            void substitute(const DeducedEquation<NumT>& deducedEquation);

            /**
             * Substitute a variable in the equation by a constant.
             * @param assignment
             *      The assignment to use for substitution.
             */
            void substitute(const Assignment<NumT>& assignment);


            friend std::ostream& operator<<(std::ostream& os, const Equation<NumT>& eq)
            {
                os << eq.leftSide << " = " << eq.rightSide;
                return os;
            }

            const Sum<NumT>& getLeftSide() const;
            const NumT& getRightSide() const;

        private:
            Sum<NumT> leftSide;
            NumT rightSide;
    };
}
