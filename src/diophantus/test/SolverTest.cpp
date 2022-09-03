#include <diophantus/Solver.hpp>
#include <diophantus/Validator.hpp>

#include <diophantus/model/Assignment.hpp>
#include <diophantus/model/Sum.hpp>
#include <diophantus/model/Term.hpp>
#include <diophantus/model/Variable.hpp>
#include <diophantus/model/Solution.hpp>
#include <diophantus/model/util.hpp>
#include <diophantus/model/numeric/GmpBigInt.hpp>
#include <diophantus/model/Equation.hpp>
#include <diophantus/model/EquationSystem.hpp>

#include <common/logging.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <optional>
#include <vector>


using NumT = diophantus::model::numeric::GmpBigInt;
using Variable = diophantus::model::Variable;

using Assignment = diophantus::model::Assignment<NumT>;
using Equation = diophantus::model::Equation<NumT>;
using EquationSystem = diophantus::model::EquationSystem<NumT>;
using Solution = diophantus::model::Solution<NumT>;
using Sum = diophantus::model::Sum<NumT>;
using Term = diophantus::model::Term<NumT>;

using Solver = diophantus::Solver<NumT>;
using Validator = diophantus::Validator<NumT>;


TEST(SolverTest, SimpleSystem)
{
    size_t nVariables = 3;
    auto variables = diophantus::model::make_variables(nVariables);

    auto equation1 = diophantus::model::makeEquation<NumT>(variables, {7, 12, 31}, 17);
    auto equation2 = diophantus::model::makeEquation<NumT>(variables, {3, 5, 14}, 7);

    auto equationSystem = EquationSystem(variables, {equation1, equation2});

    diophantus::Solver<NumT> solver(equationSystem);
    std::optional<Solution> solution = solver.solve();

    EXPECT_TRUE(solution.has_value());

    if (solution.has_value())
    {
        Validator val(equationSystem);
        EXPECT_TRUE(val.isValidSolution(solution.value()));
    }
}

TEST(SolverTest, Unsolvable)
{
    size_t nVariables = 3;
    auto variables = diophantus::model::make_variables(nVariables);

    auto equation1 = diophantus::model::makeEquation<NumT>(variables, {7, 21, 28}, 8);
    auto equation2 = diophantus::model::makeEquation<NumT>(variables, {3, 5, 14}, 7);

    auto equationSystem = EquationSystem(variables, {equation1, equation2});

    diophantus::Solver<NumT> solver(equationSystem);
    std::optional<Solution> solution = solver.solve();

    EXPECT_FALSE(solution.has_value());
}

TEST(SolverTest, Empty)
{
    size_t nVariables = 0;
    auto variables = diophantus::model::make_variables(nVariables);

    auto equation1 = diophantus::model::makeEquation<NumT>(variables, {}, 0);

    auto equationSystem = EquationSystem(variables, {equation1});

    diophantus::Solver<NumT> solver(equationSystem);
    std::optional<Solution> solution = solver.solve();

    EXPECT_TRUE(solution.has_value());
    EXPECT_TRUE(solution->assignments.empty());
}

TEST(SolverTest, Conflict)
{
    size_t nVariables = 1;
    auto variables = diophantus::model::make_variables(nVariables);

    auto equation1 = diophantus::model::makeEquation<NumT>(variables, {0}, 1);

    auto equationSystem = EquationSystem(variables, {equation1});

    diophantus::Solver<NumT> solver(equationSystem);
    std::optional<Solution> solution = solver.solve();

    EXPECT_FALSE(solution.has_value());
}
