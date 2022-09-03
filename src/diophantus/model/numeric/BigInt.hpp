#pragma once

#include <cstddef>
#include <string>

namespace diophantus::model::numeric
{
    template<typename Number>
    concept InitializableFromLong = requires(Number a, long i) { Number(i); };

    template<typename Number>
    concept InitializableFromString = requires(Number a, std::string s) { Number(s); };

    template<typename Number>
    concept Show = requires(Number a, std::ostream os) { os << a; };

    template<typename Number>
    concept Comparable = requires(Number a, Number b) { a <=> b; };

    template<typename Number>
    concept Arithmetic = requires(Number a, Number b)
    {
        a + b;
        a - b;
        a * b;
        // a / b;
        a % b;

        a += b;
        a -= b;
        a *= b;
        a /= b;

        -a;

        Number::abs(a);             // Get absolute value
        Number::gcd(a, b);          // Calculate greatest common divisor
        Number::symMod(a, b);       // Calculate symmetric modulo
    };

    template<typename Number>
    concept BigInt = (InitializableFromLong<Number>
                   && InitializableFromString<Number>
                   && Show<Number>
                   && Comparable<Number>
                   && Arithmetic<Number>);
}
