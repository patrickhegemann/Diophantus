#include "Validator.hpp"

#include "model/SimplificationResult.hpp"

#include "model/numeric/GmpBigInt.hpp"
#include "model/numeric/BigInt.hpp"

namespace diophantus
{
    template <model::numeric::BigInt NumT>
    Validator<NumT>::Validator(model::EquationSystem<NumT> equationSystem) :
        equationSystem(std::move(equationSystem))
    {}
    
    template <model::numeric::BigInt NumT>
    bool Validator<NumT>::isValidSolution(model::Solution<NumT> solution)
    {
        // Get a copy of the equation system and substitute all assignments in
        model::EquationSystem<NumT> tmpEquationSystem = equationSystem;

        for (const auto& assignment : solution.assignments)
        {
            tmpEquationSystem.substitute(assignment);
        }

        // If no equations are left to be solved and there is no conflict, the solution is valid
        auto result = tmpEquationSystem.simplify();
        return result == model::SimplificationResult::IsEmpty;
    }

    template class Validator<model::numeric::GmpBigInt>;
}
