#include <diophantus/Validator.hpp>

#include <diophantus/model/numeric/GmpBigInt.hpp>
#include <diophantus/model/Assignment.hpp>
#include <diophantus/model/Sum.hpp>
#include <diophantus/model/Term.hpp>
#include <diophantus/model/Variable.hpp>
#include <diophantus/model/Solution.hpp>
#include <diophantus/model/util.hpp>
#include <diophantus/model/Equation.hpp>
#include <diophantus/model/EquationSystem.hpp>

#include <common/logging.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <vector>


using NumT = diophantus::model::numeric::GmpBigInt;
using Variable = diophantus::model::Variable;

using Assignment = diophantus::model::Assignment<NumT>;
using Equation = diophantus::model::Equation<NumT>;
using EquationSystem = diophantus::model::EquationSystem<NumT>;
using Solution = diophantus::model::Solution<NumT>;
using Sum = diophantus::model::Sum<NumT>;
using Term = diophantus::model::Term<NumT>;

using Validator = diophantus::Validator<NumT>;


class ValidatorTest:
    public ::testing::Test
{
    protected:
        ValidatorTest()
        {
            size_t nVariables = 3;
            variables = diophantus::model::make_variables(nVariables);
            auto equation1 = diophantus::model::makeEquation<NumT>(variables, {7, 12, 31}, 17);
            auto equation2 = diophantus::model::makeEquation<NumT>(variables, {3, 5, 14}, 7);
            auto equationSystem = EquationSystem(variables, {equation1, equation2});

            validator = std::make_unique<Validator>(equationSystem);
        }

        std::vector<Variable> variables;
        std::unique_ptr<Validator> validator;
};

TEST_F(ValidatorTest, ValidSolution)
{
    Solution validSolution {
        .assignments = {
            Assignment {.variable = variables[0], .value = NumT(12)},
            Assignment {.variable = variables[1], .value = NumT(-3)},
            Assignment {.variable = variables[2], .value = NumT(-1)},
        }
    };

    EXPECT_TRUE(validator->isValidSolution(validSolution));
}

TEST_F(ValidatorTest, InvalidSolution)
{
    Solution invalidSolution {
        .assignments = {
            Assignment {.variable = variables[0], .value = NumT(1)},
            Assignment {.variable = variables[1], .value = NumT(2)},
            Assignment {.variable = variables[2], .value = NumT(-7)},
        }
    };

    EXPECT_FALSE(validator->isValidSolution(invalidSolution));
}
