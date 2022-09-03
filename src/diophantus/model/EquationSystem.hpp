#pragma once

#include "Assignment.hpp"
#include "DeducedEquation.hpp"
#include "Equation.hpp"
#include "SimplificationResult.hpp"

#include "numeric/GmpBigInt.hpp"
#include "numeric/BigInt.hpp"

#include <logging.hpp>

#include <algorithm>
#include <execution>
#include <list>
#include <memory>
#include <pstl/glue_execution_defs.h>
#include <vector>

namespace diophantus::model
{
    template <numeric::BigInt NumT>
    class EquationSystem
    {
        public:
            EquationSystem(const std::vector<std::shared_ptr<Variable>>& variables,
                           const std::vector<Equation<NumT>>& equations);

            // TODO: Better data structure?
            std::vector<Equation<NumT>>& getEquations();

            unsigned int getVariableCount() const;
            size_t getEquationCount() const;

            /**
             * Creates a new variable for use in the equation system.
             * @return pointer to the newly created variable
             */
            const std::shared_ptr<Variable> addNewVariable();

            /**
             * Substitute a variable by applying an assignment to all equations.
             * @param assignment
             *      The assignment used for substitution
             */
            void substitute(const Assignment<NumT>& assignment);

            /**
             * Substitute a variable by a deduced equation, for all equations.
             * @param deducedEquation
             *      The deduced equation used for substitution
             */
            void substitute(const DeducedEquation<NumT>& deducedEquation);

            /**
             * Simplify the equation system by deleting duplicate equations and simplifying all
             * other equations.
             */
            SimplificationResult simplify();


            friend std::ostream& operator<<(std::ostream& os, const EquationSystem<NumT>& system)
            {
                if (system.equations.empty())
                {
                    os << "Empty equation system";
                    return os;
                }

                // Stream first equation (without leading newline)
                auto eq = system.equations.begin();
                os << *eq;
                eq++;

                // Stream all other equations (each with a leading newline)
                for (; eq != system.equations.end(); ++eq)
                {
                    os << std::endl << *eq;
                }
                return os;
            }

        private:
            std::vector<std::shared_ptr<Variable>> variables;
            std::vector<Equation<NumT>> equations;
    };
}


namespace diophantus::model
{
    template <numeric::BigInt NumT>
    EquationSystem<NumT>::EquationSystem(const std::vector<std::shared_ptr<Variable>>& variables,
                                         const std::vector<Equation<NumT>>& equations) :
        variables(std::move(variables)),
        equations(std::move(equations))
    {}

    template <numeric::BigInt NumT>
    // std::list<Equation<NumT>>& EquationSystem<NumT>::getEquations()
    std::vector<Equation<NumT>>& EquationSystem<NumT>::getEquations()
    {
        return equations;
    }

    template <numeric::BigInt NumT>
    unsigned int EquationSystem<NumT>::getVariableCount() const
    {
        return variables.size();
    }

    template <numeric::BigInt NumT>
    size_t EquationSystem<NumT>::getEquationCount() const
    {
        return equations.size();
    }

    template <numeric::BigInt NumT>
    const std::shared_ptr<Variable> EquationSystem<NumT>::addNewVariable()
    {
        unsigned int newVarNumber = variables.size();
        auto newVar = std::make_shared<Variable>(newVarNumber);
        variables.push_back(newVar);
        return newVar;
    }

    template <numeric::BigInt NumT>
    void EquationSystem<NumT>::substitute(const Assignment<NumT>& assignment)
    {
        for (auto& eq : equations)
        {
            eq.substitute(assignment);
        }
    }

    template <numeric::BigInt NumT>
    void EquationSystem<NumT>::substitute(const DeducedEquation<NumT>& deducedEquation)
    {
        for (auto& eq : equations)
        {
            eq.substitute(deducedEquation);
        }
    }

    template <numeric::BigInt NumT>
    SimplificationResult EquationSystem<NumT>::simplify()
    {
        auto eqIterator = equations.begin();
        while (eqIterator != equations.end())
        {
            const SimplificationResult result = eqIterator->simplify();
            switch (result) {
                case SimplificationResult::Conflict:
                    return SimplificationResult::Conflict;
                    break;
                
                case SimplificationResult::IsEmpty:
                    eqIterator = equations.erase(eqIterator);
                    break;

                case SimplificationResult::Ok:
                    ++eqIterator;
                    break;
            }
        }

        return equations.empty() ? SimplificationResult::IsEmpty
                                 : SimplificationResult::Ok;
    }
}
