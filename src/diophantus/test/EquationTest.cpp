#include "diophantus/model/SimplificationResult.hpp"
#include <diophantus/model/Assignment.hpp>
#include <diophantus/model/Sum.hpp>
#include <diophantus/model/Term.hpp>
#include <diophantus/model/Variable.hpp>
#include <diophantus/model/util.hpp>
#include <diophantus/model/Equation.hpp>

#include <diophantus/model/numeric/GmpBigInt.hpp>

#include <common/logging.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <vector>

using NumT = diophantus::model::numeric::GmpBigInt;

using Variable = diophantus::model::Variable;
using Term = diophantus::model::Term<NumT>;
using Sum = diophantus::model::Sum<NumT>;
using Equation = diophantus::model::Equation<NumT>;
using Assignment = diophantus::model::Assignment<NumT>;

using diophantus::model::SimplificationResult;

TEST(EquationTest, LowestCoefficient)
{
    auto variables = diophantus::model::make_variables(3);
    std::vector<Term> terms = {
        Term(1, variables[0]),
        Term(5, variables[1]),
        Term(-4, variables[2])
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 10);

    EXPECT_EQ(equation.getLowestCoefficientTerm().getCoefficient(), 1);
}

TEST(EquationTest, LowestCoefficientWithZero)
{
    auto variables = diophantus::model::make_variables(3);
    std::vector<Term> terms = {
        Term(1, variables[0]),
        Term(0, variables[1]),
        Term(-4, variables[2])
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 10);

    EXPECT_EQ(equation.getLowestCoefficientTerm().getCoefficient(), 1);
}

TEST(EquationTest, HighestCoefficient)
{
    auto variables = diophantus::model::make_variables(3);
    std::vector<Term> terms = {
        Term(0, variables[0]),
        Term(-5, variables[1]),
        Term(4, variables[2])
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 10);

    EXPECT_EQ(equation.getHighestCoefficientTerm().getCoefficient(), -5);
}

TEST(EquationTest, SimplifySimple)
{
    auto variables = diophantus::model::make_variables(3);
    std::vector<Term> terms = {
        Term(4, variables[0]),
        Term(8, variables[1]),
        Term(16, variables[2])
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 24);

    auto result = equation.simplify();
    EXPECT_EQ(result, SimplificationResult::Ok);

    EXPECT_EQ(equation.getLowestCoefficientTerm().getCoefficient(), 1);
    EXPECT_EQ(equation.getRightSide(), 6);
}

TEST(EquationTest, SimplifyPrimeCoefficient)
{
    auto variables = diophantus::model::make_variables(3);
    std::vector<Term> terms = {
        Term(4, variables[0]),
        Term(8, variables[1]),
        Term(7, variables[2])
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 24);

    const NumT& lowestCoefficientBefore = equation.getLowestCoefficientTerm().getCoefficient();

    auto result = equation.simplify();
    EXPECT_EQ(result, SimplificationResult::Ok);

    EXPECT_EQ(equation.getLowestCoefficientTerm().getCoefficient(), lowestCoefficientBefore);
    EXPECT_EQ(equation.getRightSide(), 24);
}

TEST(EquationTest, SimplifyZeroCoefficient)
{
    auto variables = diophantus::model::make_variables(3);
    std::vector<Term> terms = {
        Term(4, variables[0]),
        Term(8, variables[1]),
        Term(0, variables[2])
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 24);

    auto result = equation.simplify();
    EXPECT_EQ(result, SimplificationResult::Ok);

    EXPECT_EQ(equation.getLowestCoefficientTerm().getCoefficient(), 1);
    EXPECT_EQ(equation.getRightSide(), 6);
}

TEST(EquationTest, SimplifyConflictByGCD)
{
    auto variables = diophantus::model::make_variables(3);
    std::vector<Term> terms = {
        Term(2, variables[0]),
        Term(4, variables[1]),
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 7);

    auto result = equation.simplify();
    EXPECT_EQ(result, SimplificationResult::Conflict);
}

TEST(EquationTest, SimplifyConflictByInequality)
{
    auto variables = diophantus::model::make_variables(3);
    std::vector<Term> terms = {
        Term(0, variables[0]),
        Term(0, variables[1]),
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 1);

    auto result = equation.simplify();
    EXPECT_EQ(result, SimplificationResult::Conflict);
}

TEST(EquationTest, SimplifyEmpty)
{
    auto variables = diophantus::model::make_variables(3);
    std::vector<Term> terms = {
        Term(0, variables[0]),
        Term(0, variables[1]),
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 0);

    auto result = equation.simplify();
    EXPECT_EQ(result, SimplificationResult::IsEmpty);
}

TEST(EquationTest, Invert)
{
    auto variables = diophantus::model::make_variables(3);
    std::vector<Term> terms = {
        Term(-4, variables[0]),
        Term(8, variables[1]),
        Term(7, variables[2])
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 24);

    equation.invert();

    EXPECT_EQ(equation.getLowestCoefficientTerm().getCoefficient(), 4);
    EXPECT_EQ(equation.getRightSide(), -24);

    const auto& newTerms = equation.getLeftSide().getTerms();
    EXPECT_EQ(newTerms.size(), equation.getLeftSide().getTerms().size());

    EXPECT_EQ(newTerms[0].getCoefficient(), 4);
    EXPECT_EQ(newTerms[0].getVariable()->variableNumber, 0);

    EXPECT_EQ(newTerms[1].getCoefficient(), -8);
    EXPECT_EQ(newTerms[1].getVariable()->variableNumber, 1);

    EXPECT_EQ(newTerms[2].getCoefficient(), -7);
    EXPECT_EQ(newTerms[2].getVariable()->variableNumber, 2);
}

TEST(EquationTest, SolveFor)
{
    auto variables = diophantus::model::make_variables(3);
    std::vector<Term> terms = {
        Term(1, variables[0]),
        Term(5, variables[1]),
        Term(-4, variables[2])
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 10);

    const auto& term = equation.getLeftSide().getTerms()[0];
    const auto deducedEquation = equation.solveFor(term);

    EXPECT_EQ(deducedEquation.getRightSideConstant(), 10);
    EXPECT_EQ(deducedEquation.getVariable(), term.getVariable());

    const auto& rightSideTerms = deducedEquation.getRightSideSum().getTerms();
    EXPECT_EQ(rightSideTerms.size(), equation.getLeftSide().getTerms().size() - 1);

    // TODO: Think of better method for testing this:
    //      - equation.hasTerm ?
    //      - equaton.getTermOfVariable ?
    //      - variable.terms.at(equation)->getCoefficient() ?
    EXPECT_EQ(rightSideTerms[0].getCoefficient(), -5);
    EXPECT_EQ(rightSideTerms[0].getVariable()->variableNumber, 1);

    EXPECT_EQ(rightSideTerms[1].getCoefficient(), 4);
    EXPECT_EQ(rightSideTerms[1].getVariable()->variableNumber, 2);
}

TEST(EquationTest, SolveForWithInversion)
{
    // TODO: Will this ever occur? (negative lowest coefficient avoided by inverting the equation
    //       in the solver?)

    auto variables = diophantus::model::make_variables(3);
    std::vector<Term> terms = {
        Term(-1, variables[0]),
        Term(5, variables[1]),
        Term(-4, variables[2])
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 10);

    const auto& term = equation.getLeftSide().getTerms()[0];
    const auto deducedEquation = equation.solveFor(term);

    EXPECT_EQ(deducedEquation.getRightSideConstant(), -10);
    EXPECT_EQ(deducedEquation.getVariable(), term.getVariable());

    const auto& rightSideTerms = deducedEquation.getRightSideSum().getTerms();
    EXPECT_EQ(rightSideTerms.size(), equation.getLeftSide().getTerms().size() - 1);

    EXPECT_EQ(rightSideTerms[0].getCoefficient(), 5);
    EXPECT_EQ(rightSideTerms[0].getVariable()->variableNumber, 1);

    EXPECT_EQ(rightSideTerms[1].getCoefficient(), -4);
    EXPECT_EQ(rightSideTerms[1].getVariable()->variableNumber, 2);
}

TEST(EquationTest, Eliminate)
{
    auto variables = diophantus::model::make_variables(4);
    std::vector<Term> terms = {
        Term(7, variables[0]),
        Term(12, variables[1]),
        Term(31, variables[2])
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 17);

    const auto& term = equation.getLeftSide().getTerms()[0];
    const auto deducedEquation = equation.eliminate(term, variables[3]);

    EXPECT_EQ(deducedEquation.getRightSideConstant(), -1);
    EXPECT_EQ(deducedEquation.getVariable(), term.getVariable());

    const auto& rightSideTerms = deducedEquation.getRightSideSum().getTerms();
    EXPECT_EQ(rightSideTerms.size(), equation.getLeftSide().getTerms().size());

    EXPECT_EQ(rightSideTerms[0].getCoefficient(), -4);
    EXPECT_EQ(rightSideTerms[0].getVariable()->variableNumber, 1);

    EXPECT_EQ(rightSideTerms[1].getCoefficient(), -1);
    EXPECT_EQ(rightSideTerms[1].getVariable()->variableNumber, 2);

    EXPECT_EQ(rightSideTerms[2].getCoefficient(), -8);
    EXPECT_EQ(rightSideTerms[2].getVariable()->variableNumber, 3);
}

TEST(EquationTest, SubstituteByAssignment)
{
    auto variables = diophantus::model::make_variables(3);
    std::vector<Term> terms = {
        Term(7, variables[0]),
        Term(12, variables[1]),
        Term(31, variables[2])
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 17);

    auto assignment = Assignment{
        .variable = variables[1],
        .value = NumT(53)
    };

    equation.substitute(assignment);

    EXPECT_EQ(equation.getRightSide(), -619);

    const auto& leftSideTerms = equation.getLeftSide().getTerms();
    EXPECT_EQ(leftSideTerms.size(), 3);

    EXPECT_EQ(leftSideTerms[0].getCoefficient(), 7);
    EXPECT_EQ(leftSideTerms[0].getVariable()->variableNumber, 0);

    EXPECT_EQ(leftSideTerms[1].getCoefficient(), 0);
    EXPECT_EQ(leftSideTerms[1].getVariable()->variableNumber, 1);

    EXPECT_EQ(leftSideTerms[2].getCoefficient(), 31);
    EXPECT_EQ(leftSideTerms[2].getVariable()->variableNumber, 2);
}

TEST(EquationTest, SubstituteByDeducedEquation)
{
    auto variables = diophantus::model::make_variables(4);
    std::vector<Term> terms = {
        Term(7, variables[0]),
        Term(12, variables[1]),
        Term(31, variables[2])
    };
    auto sum = Sum(terms);
    auto equation = Equation(sum, 17);

    const auto& term = equation.getLeftSide().getTerms()[0];
    const auto deducedEquation = equation.eliminate(term, variables[3]);

    equation.substitute(deducedEquation);
    equation.simplify();

    EXPECT_EQ(equation.getRightSide(), 3);

    const auto& leftSideTerms = equation.getLeftSide().getTerms();
    EXPECT_EQ(leftSideTerms.size(), 3);

    EXPECT_EQ(leftSideTerms[0].getCoefficient(), -2);
    EXPECT_EQ(leftSideTerms[0].getVariable()->variableNumber, 1);

    EXPECT_EQ(leftSideTerms[1].getCoefficient(), 3);
    EXPECT_EQ(leftSideTerms[1].getVariable()->variableNumber, 2);

    EXPECT_EQ(leftSideTerms[2].getCoefficient(), -7);
    EXPECT_EQ(leftSideTerms[2].getVariable()->variableNumber, 3);
}
