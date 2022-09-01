#pragma once

#include "numeric/GmpBigInt.hpp"
#include "numeric/BigInt.hpp"

#include <optional>

namespace diophantus::model
{
    struct Variable
    {
        public:
            Variable(unsigned int variableNumber) :
                variableNumber(variableNumber)
            {}

            const unsigned int variableNumber;
    };
}
