#pragma once

#include "diophantus/model/numeric/GmpBigInt.hpp"
#include <diophantus/model/numeric/BigInt.hpp>
#include <diophantus/model/Equation.hpp>

#include <memory>
#include <vector>


namespace diophantus::model
{
    struct Variable;

    /**
     * Creates a vector of variables with ascending variable number going from 0 to nVariables-1.
     * @param nVariables
     *      Number of variables to create.
     * @return vector of the created variables
     */
    std::vector<std::shared_ptr<Variable>> make_variables(size_t nVariables);

    /**
     * Convenience function for creating an equation.
     * @param variables
     *      The variables to use in this equation
     * @param coefficients
     *      The coefficients aligned with the given variables.
     * @param rightSide
     *      The constant on the right side of the equation.
     * @return the created equation
     */
    template <numeric::BigInt NumT>
    Equation<NumT> makeEquation(const std::vector<std::shared_ptr<Variable>>& variables,
                                const std::vector<long>& coefficients,
                                const long rightSide);
}
