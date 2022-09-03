#pragma once

#include "Term.hpp"

#include <compare>
#include <diophantus/model/Variable.hpp>
#include <diophantus/model/numeric/GmpBigInt.hpp>
#include <diophantus/model/numeric/BigInt.hpp>

#include <logging.hpp>

#include <algorithm>
#include <bits/ranges_algo.h>
#include <execution>
#include <ios>
#include <optional>
#include <pstl/glue_algorithm_defs.h>
#include <pstl/glue_execution_defs.h>
#include <ranges>
#include <utility>
#include <vector>


namespace diophantus::model
{
    template <numeric::BigInt NumT>
    class Sum
    {
        // TODO: Chose more suitable data structures

        public:
            explicit Sum<NumT>(const std::vector<Term<NumT>>& terms) :
                terms(std::move(terms))
            {}

            const std::vector<Term<NumT>>& getTerms() const
            {
                return terms;
            }

            void addTerm(const Term<NumT>& term)
            {
                terms.push_back(std::move(term));
            }

            /**
             * Determines the term with the lowest coefficient.
             * @return 
             */
            const Term<NumT>& getLowestCoefficientTerm() const
            {
                // Find term with the minimum absolute coefficient other than zero
                auto compareAbsolute = [](const Term<NumT>& a, const Term<NumT> &b)
                {
                    bool aIsLower = a.getCoefficient().absCmp(b.getCoefficient()) == std::strong_ordering::less;
                    return aIsLower && (a.getCoefficient() != 0);
                };

                return *std::min_element(terms.begin(), terms.end(), compareAbsolute);
            }

            /**
             * Determines the term with the highest coefficient.
             * @return 
             */
            const Term<NumT>& getHighestCoefficientTerm() const
            {
                // Find term with the maximum absolute coefficient other than zero
                auto compareAbsolute = [](const Term<NumT>& a, const Term<NumT> &b)
                {
                    bool aIsLower = a.getCoefficient().absCmp(b.getCoefficient()) == std::strong_ordering::less;
                    return aIsLower || (a.getCoefficient() == 0);
                };

                return *std::max_element(terms.begin(), terms.end(), compareAbsolute);
            }

            /**
             * Simplifies the equation by:
             *      - Deleting terms with coefficient 0
             *      - Dividing all coefficients by their greatest common divisor
             * @return The greatest common divisor of all coefficients if there are terms left,
                       std::nullopt otherwise.
             */
            const std::optional<NumT> simplify()
            {
                removeZeroTerms();

                auto gcd = gcdOfCoefficients();
                if (gcd == 0)
                {
                    return std::nullopt;
                }
                return gcd;
            }

            /**
             * Divides the coefficients of all terms by a number.
             * @param divisor
             */
            void divideCoefficientsBy(const NumT& divisor)
            {
                for (Term<NumT>& term : terms)
                {
                    term.divideCoefficientBy(divisor);
                }
            }

            /**
             * Takes the coefficients of all terms modulo a number.
             * @param divisor
             */
            void coefficientsModulo(const NumT& modulus)
            {
                for (Term<NumT>& term : terms)
                {
                    term.coefficientMod(modulus);
                }

                // TODO: Handle terms with coefficient 0?
            }

            /**
             * Sets the coefficient of the specified variable to zero.
             * @param var
             *      The variable
             * @return The old coefficient of the variable, if it is present in the sum.
             *         nullopt, otherwise.
             */
            const std::optional<NumT> setCoefficientOfVariableToZero(const std::shared_ptr<const Variable> var)
            {
                // Search for the first (and only) term with this variable
                auto variableMatches = [var](const Term<NumT>& term)
                {
                    return (term.getVariable() == var);
                };
                const auto& varTerm = std::find_if(//std::execution::par,
                                                   terms.begin(), terms.end(),
                                                   variableMatches);

                if (varTerm == terms.end())
                {
                    // Term was not present in this sum, cannot return any old coefficient.
                    return std::nullopt;
                }
                else
                {
                    // Copy old coefficient, set it to zero and return old coefficient
                    // TODO: Remove term instead? Depending on the data structure
                    // return std::exchange(varTerm->coefficient, NumT(0));

                    NumT oldCoefficient{varTerm->getCoefficient()};
                    varTerm->setCoefficientToZero();
                    return oldCoefficient;
                }
            }

            friend std::ostream& operator<<(std::ostream& os, const Sum<NumT>& sum)
            {
                // C++23 -> std::ranges::views::drop_last | std::ranges::accumulate | ...

                if (sum.terms.empty())
                {
                    os << "0";
                }

                bool firstIteration = true;
                for (const auto& term : sum.terms)
                {
                    if (!firstIteration)
                    {
                        os << " + ";
                    }
                    firstIteration = false;

                    if (!term.getVariable())
                    {
                        os << "WARNING: VARIABLE MISSING";
                    }
                    else
                    {
                        os <<  "(" << term.getCoefficient() << ")*"
                           << "x[" << term.getVariable()->variableNumber << "]";
                    }
                }

                return os;
            }

        private:
            const NumT gcdOfCoefficients() const
            {
                NumT gcd = NumT(0);

                bool firstIteration = true;
                for (const auto& term : terms)
                {
                    const NumT& coefficient = term.getCoefficient();
                    if (coefficient == 0)
                    {
                        continue;
                    }

                    if (firstIteration)
                    {
                        gcd = coefficient;
                        firstIteration = false;
                    }
                    else
                    {
                        gcd = NumT::gcd(gcd, coefficient);
                    }
                    
                }
                return gcd;
            }

            bool areAllCoefficientsZero() const
            {
                return std::ranges::find_if(terms, [](const Term<NumT>& term) {
                    return term.getCoefficient() != 0;
                }) == terms.end();
            }

            void removeZeroTerms()
            {
                std::erase_if(terms,
                            [](const Term<NumT>& term) {
                                return term.getCoefficient() == 0;
                            });
            }

        private:
            std::vector<Term<NumT>> terms;
    };
}
