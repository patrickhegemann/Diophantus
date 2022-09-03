#include "Solver.hpp"

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
#include <iterator>
#include <memory>
#include <optional>
#include <random>
#include <ranges>
#include <vector>

namespace diophantus
{
    template <model::numeric::BigInt NumT>
    Solver<NumT>::Solver(const model::EquationSystem<NumT>& equationSystem) :
        equationSystem(std::move(equationSystem)),
        nOriginalVariables(equationSystem.getVariableCount())
    {
    }

    template <model::numeric::BigInt NumT>
    std::optional<model::Solution<NumT>> Solver<NumT>::solve()
    {
        LOG_DEBUG << "Solving equation system: " << std::endl << equationSystem;

        // TODO: Make max. iteration count a parameter
        // constexpr unsigned int maxIterations = 100;
        // for (unsigned int i = 0; i < maxIterations; ++i)

        for (unsigned int i = 0;; ++i)
        {
            LOG_DEBUG << "Iteration " << i;
            LOG_DEBUG << "\tNumber of equations left: " << equationSystem.getEquationCount();

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
            // LOG_DEBUG << "Picked equation:" << std::endl << currentEquation;

            const auto newEquation = deduceNewEquation(currentEquation);
            // LOG_DEBUG << "Deduced equation: " << std::endl << newEquation;

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
        // TODO: Refactor and clean up
        auto& equations = equationSystem.getEquations();

        auto minEquation = equations.begin();
        NumT minCoefficient = minEquation->getLowestCoefficientTerm().getCoefficient();

        auto maxEquation = equations.begin();
        NumT maxCoefficient = maxEquation->getHighestCoefficientTerm().getCoefficient();

        // This is basically std::minmax_element...
        auto eqIterator = minEquation;
        // if (eqIterator->getLeftSide().getTerms().size() == 1)
        // {
        //     return *eqIterator;
        // }

        // eqIterator++;
        for (uint currentIndex = 0; eqIterator != equations.end(); ++eqIterator, ++currentIndex)
        {
            if (eqIterator->getLeftSide().getTerms().size() == 1)
            {
                return *eqIterator;
            }

            NumT thisMinCoefficient = eqIterator->getLowestCoefficientTerm().getCoefficient();
            NumT thisMaxCoefficient = eqIterator->getHighestCoefficientTerm().getCoefficient();
            if (NumT::abs(thisMinCoefficient) < NumT::abs(minCoefficient))
            {
                minEquation = eqIterator;
                minCoefficient = thisMinCoefficient;
            }
            else if (NumT::abs(thisMaxCoefficient) > NumT::abs(maxCoefficient))
            {
                maxEquation = eqIterator;
                maxCoefficient = thisMaxCoefficient;
            }
        }

        LOG_DEBUG << "\tmax coefficient has " << NumT::nDigits(maxCoefficient) << " digits";

        auto minCoefficientOnFirstEquation = equations.begin()->getLowestCoefficientTerm().getCoefficient();
        LOG_DEBUG << "\tmin coefficient on first equation has " << NumT::nDigits(minCoefficientOnFirstEquation) << " digits";
        auto maxCoefficientOnFirstEquation = equations.begin()->getHighestCoefficientTerm().getCoefficient();
        LOG_DEBUG << "\tmax coefficient on first equation has " << NumT::nDigits(maxCoefficientOnFirstEquation) << " digits";

        // TODO: Find a better heuristic...
        if (NumT::abs(minCoefficient) == 1)
        {
            LOG_DEBUG << "Chose equation with coefficient 1";
            return *minEquation;
        }
        else
        {
            LOG_DEBUG << "Chose first equation";
            return *equations.begin();
        }
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
            return assignment.variable->variableNumber < maxVariableNumber;
        };

        // Copy all assignments that are relevant to the original equation
        std::ranges::copy_if(assignments,
                             std::back_inserter(relevantAssignments),
                             isRelevant);

        return model::Solution<NumT> {.assignments = relevantAssignments};
    }

    template class Solver<model::numeric::GmpBigInt>;
}
