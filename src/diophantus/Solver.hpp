#pragma once

#include "model/DeducedEquation.hpp"
#include "model/Equation.hpp"
#include "model/EquationSystem.hpp"
#include "model/Solution.hpp"
#include "model/Term.hpp"
#include "model/Variable.hpp"

#include "model/numeric/GmpBigInt.hpp"
#include "model/numeric/BigInt.hpp"

#include <optional>
#include <random>

namespace diophantus
{
    template <model::numeric::BigInt NumT>
    class Solver
    {
        public:
            explicit Solver(const model::EquationSystem<NumT>& equationSystem);

            /**
             * Solves the given equation system.
             * @return A solution if the equation system is solvable, nullopt otherwise. 
             */
            std::optional<model::Solution<NumT>> solve();

        private:
            /**
             * Picks the next equation to process, considering the minimum and maximum
             * coefficients. If a variable has coefficient 1, the corresponding equation will be
             * picked. Otherwise the equation containing the highest coefficient will be picked.
             * @return The next equation to process
             */
            model::Equation<NumT>& pickEquation();

            /**
             * Deduces a new equation from the given equation by solving it for one variable.
             * @param currentEquation
             *      The equation to process
             * @return The deduced equation.
             */
            const model::DeducedEquation<NumT> deduceNewEquation(model::Equation<NumT>& currentEquation);

            /**
             * Substitute variables in the equation system by the previously deduced equations.
             */
            void backPropagateDeducedEquations();

            /**
             * Creates a solution from the deduced variable assignments.
             * @return A solution for the equation system.
             */
            const model::Solution<NumT> getSolutionFromAssignments() const;

        private:
            model::EquationSystem<NumT> equationSystem;
            size_t nOriginalVariables;

            std::vector<model::DeducedEquation<NumT>> deducedEquations;
            std::vector<model::Assignment<NumT>> assignments;

            bool chooseMaxTermNext = false;
    };
}
