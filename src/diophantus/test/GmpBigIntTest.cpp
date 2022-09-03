#include <diophantus/model/numeric/BigInt.hpp>
#include <diophantus/model/numeric/GmpBigInt.hpp>

#include <gtest/gtest-death-test.h>
#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <limits>


// Compile time check: Does GmpBigInt satisfy BigInt concept constraints?
namespace diophantus::test
{
    template<diophantus::model::numeric::BigInt NumT> class Foo {};
    using FooBigInt = Foo<diophantus::model::numeric::GmpBigInt>;
}


using Number = diophantus::model::numeric::GmpBigInt;

TEST(GmpBigIntTest, InitializerEquality)
{
    Number a{5};
    Number b{"5"};

    EXPECT_EQ(a, b);
}

TEST(GmpBigIntTest, Negation)
{
    Number a{5};
    Number b{-5};

    EXPECT_EQ(-a, b);
}

TEST(GmpBigIntTest, Multiplication)
{
    Number a{6};
    Number b{7};

    EXPECT_EQ(a*b, 42);
}

TEST(GmpBigIntTest, Modulo)
{
    Number a{23};
    Number b{4};

    EXPECT_EQ(a % b, 3);
}

TEST(GmpBigIntTest, Absolute)
{
    Number a{-5};
    EXPECT_EQ(Number::abs(a), 5);

    Number b{3};
    EXPECT_EQ(Number::abs(b), 3);
}

TEST(GmpBigIntTest, GCD)
{
    Number a{480};
    Number b{200};

    EXPECT_EQ(Number::gcd(a, b), 40);
}

TEST(GmpBigIntTest, SymMod)
{
    EXPECT_EQ(Number::symMod(Number(10), Number(5)), 0);
    EXPECT_EQ(Number::symMod(Number(11), Number(5)), 1);
    EXPECT_EQ(Number::symMod(Number(12), Number(5)), 2);
    EXPECT_EQ(Number::symMod(Number(13), Number(5)), -2);
    EXPECT_EQ(Number::symMod(Number(14), Number(5)), -1);
    EXPECT_EQ(Number::symMod(Number(15), Number(5)), 0);

    EXPECT_EQ(Number::symMod(Number(-10), Number(5)), 0);
    EXPECT_EQ(Number::symMod(Number(-11), Number(5)), -1);
    EXPECT_EQ(Number::symMod(Number(-12), Number(5)), -2);
    EXPECT_EQ(Number::symMod(Number(-13), Number(5)), 2);
    EXPECT_EQ(Number::symMod(Number(-14), Number(5)), 1);
    EXPECT_EQ(Number::symMod(Number(-15), Number(5)), 0);

    EXPECT_EQ(Number::symMod(Number(12), Number(6)), 0);
    EXPECT_EQ(Number::symMod(Number(13), Number(6)), 1);
    EXPECT_EQ(Number::symMod(Number(14), Number(6)), 2);
    EXPECT_EQ(Number::symMod(Number(15), Number(6)), -3);
    EXPECT_EQ(Number::symMod(Number(16), Number(6)), -2);
    EXPECT_EQ(Number::symMod(Number(17), Number(6)), -1);
    EXPECT_EQ(Number::symMod(Number(18), Number(6)), 0);

    EXPECT_EQ(Number::symMod(Number(-12), Number(6)), 0);
    EXPECT_EQ(Number::symMod(Number(-13), Number(6)), -1);
    EXPECT_EQ(Number::symMod(Number(-14), Number(6)), -2);
    EXPECT_EQ(Number::symMod(Number(-15), Number(6)), -3);
    EXPECT_EQ(Number::symMod(Number(-16), Number(6)), 2);
    EXPECT_EQ(Number::symMod(Number(-17), Number(6)), 1);
    EXPECT_EQ(Number::symMod(Number(-18), Number(6)), 0);

    EXPECT_EQ(Number::symMod(Number(12), Number(8)), -4);
    EXPECT_EQ(Number::symMod(Number(17), Number(8)), 1);

    ASSERT_EXIT(Number::symMod(Number(5), Number(0)),
                testing::KilledBySignal(SIGFPE), ".*");
}

TEST(GmpBigIntTest, Comparison)
{
    Number a{1};
    Number b{2};
    Number c{2};

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(b == c);
    EXPECT_TRUE(a != c);
}

TEST(GmpBigIntTest, Arithmetic)
{
    Number a{6};

    a += Number(1);
    EXPECT_EQ(a, 7);

    a -= Number(1);
    EXPECT_EQ(a, 6);

    a *= Number(7);
    EXPECT_EQ(a, 42);

    a /= Number(6);
    EXPECT_EQ(a, 7);

    EXPECT_EQ(a + a, 14);
}

TEST(GmpBigIntTest, NoOverflow)
{
    const long maxLong = std::numeric_limits<long>::max();

    Number bigIntBeforeIncrement{maxLong};

    Number bigIntAfterIncrement(maxLong);
    bigIntAfterIncrement += Number(1);

    EXPECT_GT(bigIntAfterIncrement, bigIntBeforeIncrement);
}
