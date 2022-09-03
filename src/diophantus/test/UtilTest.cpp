#include <diophantus/model/util.hpp>
#include <diophantus/model/Term.hpp>
#include <diophantus/model/Variable.hpp>

#include <diophantus/model/numeric/BigInt.hpp>
#include <diophantus/model/numeric/GmpBigInt.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <vector>

TEST(UtilTest, MakeVariables)
{
    size_t nVariables = 7;
    const auto variables = diophantus::model::make_variables(nVariables);

    EXPECT_EQ(variables.size(), nVariables);
    for (size_t i = 0; i < nVariables; ++i)
    {
        EXPECT_EQ(variables[i], i);
    }
}
