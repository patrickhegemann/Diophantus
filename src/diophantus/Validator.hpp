#pragma once

#include "diophantus/model/EquationSystem.hpp"
#include "diophantus/model/Solution.hpp"

#include "model/numeric/BigInt.hpp"

namespace diophantus
{
    template <model::numeric::BigInt NumT>
    class Validator
    {
        public:
            Validator(model::EquationSystem<NumT> equationSystem);
            bool isValidSolution(model::Solution<NumT> solution);

        private:
            const model::EquationSystem<NumT> equationSystem;
    };
}
