#include "Equation.hpp"

#include "diophantus/model/DeducedEquation.hpp"
#include "diophantus/model/Sum.hpp"
#include "diophantus/model/Term.hpp"
#include "diophantus/model/Variable.hpp"
#include "diophantus/model/numeric/GmpBigInt.hpp"

#include <common/logging.h>

#include <algorithm>
#include <bits/ranges_algo.h>
#include <iterator>
#include <list>
#include <memory>
#include <optional>
#include <pstl/glue_execution_defs.h>
#include <ranges>
#include <vector>

namespace diophantus::model
{
    template <numeric::BigInt NumT>
    Equation<NumT>::Equation(const Sum<NumT>& leftSide, const NumT& rightSide) :
        leftSide(std::move(leftSide)),
        rightSide(std::move(rightSide))
    {}

    template <numeric::BigInt NumT>
    Equation<NumT>::Equation(const Sum<NumT>& leftSide, const long rightSide) :
        leftSide(std::move(leftSide)),
        rightSide(NumT(rightSide))
    {}

    template <numeric::BigInt NumT>
    SimplificationResult Equation<NumT>::simplify()
    {
        const std::optional<NumT>& gcd = leftSide.simplify();
        if (!gcd)
        {
            return rightSide == 0 ? SimplificationResult::IsEmpty
                                  : SimplificationResult::Conflict;
        }
        
        if (rightSide % gcd.value() != 0)
        {
            // Equation system is unsolvable
            return SimplificationResult::Conflict;
        }
        else
        {
            rightSide /= gcd.value();
            leftSide.divideCoefficientsBy(gcd.value());
            return SimplificationResult::Ok;
        }
    }

    template <numeric::BigInt NumT>
    void Equation<NumT>::invert()
    {
        const NumT minusOne(-1);
        leftSide.divideCoefficientsBy(minusOne);
        rightSide *= minusOne;
    }

    template <numeric::BigInt NumT>
    DeducedEquation<NumT> Equation<NumT>::solveFor(const Term<NumT>& term, bool doNormalInversion)
    {
        std::vector<Term<NumT>> newTerms;

        // Copy all terms that do not have the same variable
        const auto& variableNumber = term.getVariable()->variableNumber;
        auto isDifferentVariable = [variableNumber](const Term<NumT>& srcTerm)
        {
            return (srcTerm.getVariable()->variableNumber != variableNumber);
        };
        std::copy_if(std::execution::par,
                     leftSide.getTerms().begin(), leftSide.getTerms().end(),
                     std::back_inserter(newTerms),
                     isDifferentVariable);

        // Invert coefficients if necessary
        bool coefficientPositive = (term.getCoefficient() > 0);
        bool doCoefficientInversion = doNormalInversion && coefficientPositive;

        if (doCoefficientInversion)
        {
            auto invertCoefficient = [](Term<NumT>& newTerm)
            {
                newTerm.divideCoefficientBy(NumT(-1));
                return newTerm;
            };
            std::transform(std::execution::par,
                           newTerms.begin(), newTerms.end(),
                           newTerms.begin(),
                           invertCoefficient);
        }
        
        // Create the full new deduced equation
        bool doConstantInversion = not(doNormalInversion) || coefficientPositive;
        NumT newRightSide = doConstantInversion ? rightSide : -rightSide;
        DeducedEquation<NumT> d(term.getVariable(), Sum<NumT>(newTerms), newRightSide);

        return d;
    }

    template <numeric::BigInt NumT>
    DeducedEquation<NumT> Equation<NumT>::eliminate(const Term<NumT>& term,
                                                    const std::shared_ptr<Variable> newVariable)
    {
        NumT modulus(std::move(term.getCoefficient() + NumT(1)));

        DeducedEquation<NumT> newEquation = this->solveFor(term, false);
        newEquation.coefficientsModulo(modulus);
        newEquation.addTerm(Term<NumT>(-modulus, newVariable));
        NumT newConstant = -NumT::modOffset(newEquation.getRightSideConstant(), modulus);
        newEquation.setRightSideConstant(newConstant);

        return newEquation;
    }

    template <numeric::BigInt NumT>
    void Equation<NumT>::substitute(const DeducedEquation<NumT>& deducedEquation)
    {
        std::optional<NumT> varCoefficient = leftSide.setCoefficientOfVariableToZero(deducedEquation.getVariable());
        if (varCoefficient == std::nullopt)
        {
            return;
        }

        const auto& equationTerms = leftSide.getTerms();
        const auto& deducedTerms = deducedEquation.getRightSideSum().getTerms();

        // Merge the two lists of terms
        // TODO: Move implementation of merging to somewhere else (depends on an implementation detail of Sum)
        auto etIterator = equationTerms.begin();
        auto dtIterator = deducedTerms.begin();
        std::vector<Term<NumT>> newTerms;

        while (dtIterator != deducedTerms.end() || etIterator != equationTerms.end())
        {
            if (dtIterator != deducedTerms.end() && etIterator != equationTerms.end()
                && dtIterator->getVariable() == etIterator->getVariable())
            {
                NumT coeff = etIterator->getCoefficient()
                             + dtIterator->getCoefficient() * varCoefficient.value();
                if (coeff != 0)
                {
                    Term<NumT> term(coeff, dtIterator->getVariable());
                    newTerms.push_back(std::move(term));
                }
                ++dtIterator;
                ++etIterator;
            }
            else if(dtIterator != deducedTerms.end() && (etIterator == equationTerms.end()
                || dtIterator->getVariable()->variableNumber
                 < etIterator->getVariable()->variableNumber))
            {
                Term<NumT> term(dtIterator->getCoefficient() * varCoefficient.value(),
                                dtIterator->getVariable());
                newTerms.push_back(std::move(term));
                ++dtIterator;
            }
            else if (etIterator != equationTerms.end())
            {
                if (etIterator->getCoefficient() != 0)
                {
                    newTerms.push_back(std::move(*etIterator));
                }
                ++etIterator;
            }
        }
        
        // Update equation
        leftSide = Sum<NumT>(newTerms);
        rightSide -= varCoefficient.value() * deducedEquation.getRightSideConstant();
    }

    template <numeric::BigInt NumT>
    void Equation<NumT>::substitute(const Assignment<NumT>& assignment)
    {
        std::optional<NumT> coefficient = leftSide.setCoefficientOfVariableToZero(assignment.variable);
        if (coefficient == std::nullopt)
        {
            return;
        }
        rightSide -= coefficient.value() * assignment.value;
    }

    template <numeric::BigInt NumT>
    const Term<NumT>& Equation<NumT>::getLowestCoefficientTerm() const
    {
        return leftSide.getLowestCoefficientTerm();
    }

    template <numeric::BigInt NumT>
    const Term<NumT>& Equation<NumT>::getHighestCoefficientTerm() const
    {
        return leftSide.getHighestCoefficientTerm();
    }

    template <numeric::BigInt NumT>
    const Sum<NumT>& Equation<NumT>::getLeftSide() const
    {
        return leftSide;
    }

    template <numeric::BigInt NumT>
    const NumT& Equation<NumT>::getRightSide() const
    {
        return rightSide;
    }

    template class Equation<numeric::GmpBigInt>;
}
