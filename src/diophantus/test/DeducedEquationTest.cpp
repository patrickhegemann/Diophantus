#include <diophantus/model/Assignment.hpp>
#include <diophantus/model/DeducedEquation.hpp>
#include <diophantus/model/Sum.hpp>
#include <diophantus/model/Term.hpp>
#include <diophantus/model/Variable.hpp>
#include <diophantus/model/util.hpp>

#include <diophantus/model/numeric/GmpBigInt.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <vector>

using NumT = diophantus::model::numeric::GmpBigInt;
using Variable = diophantus::model::Variable;
using Term = diophantus::model::Term<NumT>;
using Sum = diophantus::model::Sum<NumT>;
using DeducedEquation = diophantus::model::DeducedEquation<NumT>;
using Assignment = diophantus::model::Assignment<NumT>;


class DeducedEquationTest:
    public ::testing::Test
{
    protected:
        DeducedEquationTest()
        {
            variables = diophantus::model::make_variables(3);
            std::vector<Term> terms = {
                Term(5, variables[1]),
                Term(6, variables[2]),
            };
            auto sum = Sum(terms);
            deducedEquation = std::make_unique<DeducedEquation>(variables[0], sum, NumT(3));
        }

        std::vector<std::shared_ptr<Variable>> variables;
        std::unique_ptr<DeducedEquation> deducedEquation;
};


TEST_F(DeducedEquationTest, ModuloTest)
{
    const auto& rightSideTerms = deducedEquation->getRightSideSum().getTerms();
    size_t sizeBefore = rightSideTerms.size();

    deducedEquation->coefficientsModulo(NumT(4));

    EXPECT_EQ(rightSideTerms.size(), sizeBefore);
    EXPECT_EQ(rightSideTerms[0].getCoefficient(), 1);
    EXPECT_EQ(rightSideTerms[1].getCoefficient(), -2);
}

TEST_F(DeducedEquationTest, AddTerm)
{
    constexpr unsigned int newVarNumber = 3;
    constexpr long coefficient = 17;

    auto newVar = std::make_shared<Variable>(newVarNumber);
    Term t(coefficient, newVar);

    const auto& rightSideTerms = deducedEquation->getRightSideSum().getTerms();
    size_t sizeBefore = rightSideTerms.size();

    deducedEquation->addTerm(t);    

    EXPECT_EQ(rightSideTerms.size(), sizeBefore + 1);
    EXPECT_EQ(rightSideTerms.back().getCoefficient(), coefficient);
    EXPECT_EQ(rightSideTerms.back().getVariable()->variableNumber, newVarNumber);
}

TEST_F(DeducedEquationTest, Substitute)
{
    Assignment assignment {
        .variable = variables[1],
        .value = NumT(7)
    };

    deducedEquation->substitute(assignment);

    EXPECT_EQ(deducedEquation->getRightSideConstant(), 38);

    const auto& rightSideTerms = deducedEquation->getRightSideSum().getTerms();
    EXPECT_EQ(rightSideTerms[0].getCoefficient(), 0);
    EXPECT_EQ(rightSideTerms[0].getVariable()->variableNumber, 1);
    EXPECT_EQ(rightSideTerms[1].getCoefficient(), 6);
    EXPECT_EQ(rightSideTerms[1].getVariable()->variableNumber, 2);
}
