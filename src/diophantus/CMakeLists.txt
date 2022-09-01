add_library(diophantus
    model/numeric/BigInt.hpp
    model/numeric/GmpBigInt.hpp

    model/SimplificationResult.hpp
    model/Variable.hpp
    model/Term.hpp
    model/Term.cpp
    model/Assignment.hpp
    model/Sum.hpp
    model/DeducedEquation.hpp
    model/Equation.hpp
    model/Equation.cpp
    model/EquationSystem.hpp
    model/util.hpp
    model/util.cpp
    model/Solution.hpp

    Solver.hpp
    Solver.cpp

    Validator.hpp
    Validator.cpp
)

target_link_libraries(diophantus
    gmp gmpxx
    dio_common
)

target_include_directories(diophantus
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_subdirectory(test)