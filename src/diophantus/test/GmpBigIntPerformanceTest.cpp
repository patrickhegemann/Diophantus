#include <diophantus/model/numeric/BigInt.hpp>
#include <diophantus/model/numeric/GmpBigInt.hpp>

#include <gtest/gtest.h>

#include <chrono>

using Number = diophantus::model::numeric::GmpBigInt;

TEST(GmpBigIntPerformanceTest, ModuloTestMeasureTime)
{
    unsigned int nCalls = 100000;
    std::vector<Number> ints;
    ints.reserve(nCalls);
    for (unsigned int i = 0; i < nCalls; ++i) {
        Number number(i);
        ints.push_back(std::move(number));
    }
    Number modulus(42);

    auto startTime = std::chrono::steady_clock::now();
    
    for (unsigned int i = 0; i < nCalls; ++i) {
        Number::symMod(ints[i], modulus);
    }

    auto endTime = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    std::cout << "Measured time for " << nCalls << " calls of Number::symMod: "
              << duration << " us" << std::endl;

    float averageCallTime = duration / static_cast<float>(nCalls);
    std::cout << "Average call time: " << averageCallTime << " us" << std::endl;
}
