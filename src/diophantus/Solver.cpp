#include "Solver.hpp"

#include "diophantus/model/numeric/GmpBigInt.hpp"
#include "model/Assignment.hpp"
#include "model/Equation.hpp"
#include "model/EquationSystem.hpp"
#include "model/SimplificationResult.hpp"
#include "model/Solution.hpp"
#include "model/Term.hpp"
#include "model/Variable.hpp"

#include <common/logging.hpp>

#include <algorithm>
#include <bits/ranges_algo.h>
#include <compare>
#include <iterator>
#include <memory>
#include <optional>
#include <random>
#include <ranges>
#include <vector>

namespace diophantus
{
    template <model::numeric::BigInt NumT>
    Solver<NumT>::Solver(const model::EquationSystem<NumT>& equationSystem, const Parameters& parameters) :
        parameters(std::move(parameters)),
        equationSystem(std::move(equationSystem)),
        nOriginalVariables(equationSystem.getVariableCount()),
        nOriginalEquations(equationSystem.getEquationCount()),
        lastIterationNumberOfEquations(equationSystem.getEquationCount())
    {
    }

    template <model::numeric::BigInt NumT>
    std::optional<model::Solution<NumT>> Solver<NumT>::solve()
    {
        LOG_DEBUG << "Solving equation system: " << std::endl << equationSystem;

        for (unsigned int i = 0;; ++i)
        {
            LOG_DEBUG << "Iteration " << i;

            model::SimplificationResult result = equationSystem.simplify();
            if (result == model::SimplificationResult::Conflict)
            {
                return std::nullopt;
            }
            else if (result == model::SimplificationResult::IsEmpty)
            {
                break;
            }

            model::Equation<NumT>& currentEquation = pickEquation();

            const auto newEquation = deduceNewEquation(currentEquation);

            // TODO: Make more expressive
            if (newEquation.getRightSideSum().getTerms().size() == 0)
            {
                model::Assignment<NumT> assignment{
                    .variable = newEquation.getVariable(),
                    .value = newEquation.getRightSideConstant()
                };
                equationSystem.substitute(assignment);
                assignments.push_back(assignment);
            }
            else
            {
                equationSystem.substitute(newEquation);
                deducedEquations.push_back(newEquation);
            }

            size_t nEquationsLeft = equationSystem.getEquationCount();
            if (parameters.doShowProgress && nEquationsLeft < lastIterationNumberOfEquations)
            {
                LOG_INFO << "Progress: " << nEquationsLeft << " / " << nOriginalEquations << " (Iteration " << i << ")";
            }
            lastIterationNumberOfEquations = nEquationsLeft;
        }

        LOG_DEBUG << "Resubstituting...";

        backPropagateDeducedEquations();

        LOG_DEBUG << "Extracting solution...";
        auto solution = getSolutionFromAssignments();

        return solution;
    }

    template <model::numeric::BigInt NumT>
    model::Equation<NumT>& Solver<NumT>::pickEquation()
    {
        auto& equations = equationSystem.getEquations();

        auto minEquation = equations.begin();
        NumT minCoefficient = minEquation->getLowestCoefficientTerm().getCoefficient();

        auto eqIterator = minEquation;

        for (uint currentIndex = 0; eqIterator != equations.end(); ++eqIterator, ++currentIndex)
        {
            if (eqIterator->getLeftSide().getTerms().size() == 1)
            {
                return *eqIterator;
            }

            NumT thisMinCoefficient = eqIterator->getLowestCoefficientTerm().getCoefficient();
            if (thisMinCoefficient.absCmp(minCoefficient) == std::strong_ordering::less)
            {
                minEquation = eqIterator;
                minCoefficient = thisMinCoefficient;
            }
        }

        return *minEquation;
    }

    template <model::numeric::BigInt NumT>
    const model::DeducedEquation<NumT> Solver<NumT>::deduceNewEquation(
        model::Equation<NumT>& currentEquation)
    {
        const auto& currentTerm = currentEquation.getLowestCoefficientTerm();

        // Ensure that the lowest coefficient is positive
        if (currentTerm.getCoefficient() < 0)
        {
            currentEquation.invert();
        }

        if (currentTerm.getCoefficient() == 1)
        {
            return currentEquation.solveFor(currentTerm);
        }
        else
        {
            // Create new variable and eliminate variable with lowest coefficient
            auto newVariable = equationSystem.addNewVariable();
            return currentEquation.eliminate(currentTerm, newVariable);
        }
    }

    template <model::numeric::BigInt NumT>
    void Solver<NumT>::backPropagateDeducedEquations()
    {
        // for (auto& deducedEquation : std::views::reverse(deducedEquations))
        for (auto de = deducedEquations.rbegin(); de != deducedEquations.rend(); ++de)
        {
            // Substitute variables where the value is already known
            for (const auto& assignment : assignments)
            {
                de->substitute(assignment);
            }

            // Set all other variables in this equation to zero
            for (const auto& term : de->getRightSideSum().getTerms())
            {
                if (term.getCoefficient() != 0)
                {
                    model::Assignment<NumT> zeroAssignment {
                        .variable = term.getVariable(),
                        .value = std::move(NumT(0))
                    };
                    assignments.push_back(std::move(zeroAssignment));
                }
            }

            // Set the left hand side variable to the right hand side constant
            model::Assignment<NumT> varAssignment {
                .variable = de->getVariable(),
                .value = de->getRightSideConstant()
            };
            assignments.push_back(std::move(varAssignment));
        }
    }

    template <model::numeric::BigInt NumT>
    const model::Solution<NumT> Solver<NumT>::getSolutionFromAssignments() const
    {
        std::vector<model::Assignment<NumT>> relevantAssignments;

        // An assignment is relevant if its variable appears in the original equation system
        auto maxVariableNumber = nOriginalVariables;
        auto isRelevant = [maxVariableNumber](const auto& assignment) {
            return assignment.variable < maxVariableNumber;
        };

        // Copy all assignments that are relevant to the original equation
        std::ranges::copy_if(assignments,
                             std::back_inserter(relevantAssignments),
                             isRelevant);

        return model::Solution<NumT> {.assignments = relevantAssignments};
    }

    template class Solver<model::numeric::GmpBigInt>;
}
