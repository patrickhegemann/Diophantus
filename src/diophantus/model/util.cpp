#include "util.hpp"

#include <diophantus/model/Variable.hpp>
#include <diophantus/model/Equation.hpp>
#include <diophantus/model/Term.hpp>

#include <cassert>


namespace diophantus::model
{
    std::vector<std::shared_ptr<Variable>> make_variables(size_t nVariables)
    {
        std::vector<std::shared_ptr<Variable>> variables;
        variables.reserve(nVariables);

        for (size_t i = 0; i < nVariables; ++i)
        {
            const auto variable = std::make_shared<Variable>(static_cast<unsigned int>(i));
            variables.push_back(std::move(variable));
        }

        return variables;
    }

    template <numeric::BigInt NumT>
    Equation<NumT> makeEquation(const std::vector<std::shared_ptr<Variable>>& variables,
                      const std::vector<long>& coefficients,
                      const long rightSide)
    {
        assert(coefficients.size() == variables.size());

        std::vector<Term<NumT>> terms;
        for (size_t i = 0; i < coefficients.size(); ++i)
        {
            if (coefficients[i] != 0)
            {
                Term<NumT> t(coefficients[i], variables[i]);
                terms.push_back(std::move(t));
            }
        };
        return Equation(Sum(terms), rightSide);
    }

    template Equation<numeric::GmpBigInt> makeEquation<numeric::GmpBigInt>(
        const std::vector<std::shared_ptr<Variable>> &variables,
        const std::vector<long> &coefficients,
        const long rightSide);
}
