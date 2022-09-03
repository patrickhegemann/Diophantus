#pragma once

#include <cstdint>
#include <gmp.h>
#include <gmpxx.h>

#include <compare>

namespace diophantus::model::numeric
{
    class GmpBigInt
    {
        public:
            explicit GmpBigInt(const mpz_class& value) : value(std::move(value)) {}
            explicit GmpBigInt(const long i) : value(i) {}
            explicit GmpBigInt(const std::string& s) : value(std::move(s)) {}

            // Comparable
            std::strong_ordering operator<=>(const GmpBigInt& other) const
            {
                if (value > other.value)
                {
                    return std::strong_ordering::greater;
                }
                else if (value < other.value)
                {
                    return std::strong_ordering::less;
                }
                else
                {
                    return std::strong_ordering::equal;
                }
            }

            std::strong_ordering operator<=>(const long other) const
            {
                if (value > other)
                {
                    return std::strong_ordering::greater;
                }
                else if (value < other)
                {
                    return std::strong_ordering::less;
                }
                else
                {
                    return std::strong_ordering::equal;
                }
            }
            
            bool operator==(const GmpBigInt&) const = default;

            bool operator==(const long otherValue) const
            {
                return value == otherValue;
            }

            // Arithmetic
            GmpBigInt operator+(const GmpBigInt& other) const
            {
                return GmpBigInt(value + other.value);
            }

            GmpBigInt operator-(const GmpBigInt& other) const
            {
                return GmpBigInt(value - other.value);
            }

            GmpBigInt operator*(const GmpBigInt& other) const
            {
                return GmpBigInt(value * other.value);
            }

            GmpBigInt operator%(const GmpBigInt& other) const
            {
                return GmpBigInt(value % other.value);
            }

            void operator+=(const GmpBigInt& other)
            {
                value += other.value;
            }

            void operator-=(const GmpBigInt& other)
            {
                value -= other.value;
            }

            void operator*=(const GmpBigInt& other)
            {
                value *= other.value;
            }

            void operator/=(const GmpBigInt& other)
            {
                value /= other.value;
            }

            GmpBigInt operator-() const
            {
                return GmpBigInt(-value);
            }

            static const GmpBigInt abs(const GmpBigInt& a)
            {
                mpz_class absVal;
                mpz_abs(absVal.get_mpz_t(), a.value.get_mpz_t());                
                return GmpBigInt(absVal);
            }

            // Calculate greatest common divisor
            static const GmpBigInt gcd(const GmpBigInt& a, const GmpBigInt& b)
            {
                mpz_class gcd;
                mpz_gcd(gcd.get_mpz_t(), a.value.get_mpz_t(), b.value.get_mpz_t());
                return GmpBigInt(gcd);
            }

            // Calculate symmetric modulo
            static const GmpBigInt symMod(const GmpBigInt& a, const GmpBigInt& b)
            {
                mpf_class halfB(b.value);
                halfB /= 2;

                mpz_class aModB;
                mpz_mod(aModB.get_mpz_t(), a.value.get_mpz_t(), b.value.get_mpz_t());
                if (aModB < halfB)
                {
                    return GmpBigInt(aModB);
                }
                else
                {
                    return GmpBigInt(aModB) - b;
                }
            }

            static size_t nDigits(const GmpBigInt& a)
            {
                return mpz_sizeinbase(a.value.get_mpz_t(), 10);
            }

            // Stream operator
            friend std::ostream& operator<<(std::ostream& os, const GmpBigInt& bigInt)
            {
                os << bigInt.value;
                return os;
            }

        private:
            mpz_class value;
    };
}
