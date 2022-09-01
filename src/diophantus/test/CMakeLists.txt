dio_test_case(GmpBigIntTest
    TEST_SOURCES
        GmpBigIntTest.cpp
    TEST_LIBRARIES
        diophantus
)

dio_test_case(GmpBigIntPerformanceTest
    TEST_SOURCES
        GmpBigIntPerformanceTest.cpp
    TEST_LIBRARIES
        diophantus
)

dio_test_case(DeducedEquationTest
    TEST_SOURCES
        DeducedEquationTest.cpp
    TEST_LIBRARIES
        diophantus
)

dio_test_case(EquationTest
    TEST_SOURCES
        EquationTest.cpp
    TEST_LIBRARIES
        diophantus
)

dio_test_case(EquationSystemTest
    TEST_SOURCES
        EquationSystemTest.cpp
    TEST_LIBRARIES
        diophantus
)

dio_test_case(UtilTest
    TEST_SOURCES
        UtilTest.cpp
    TEST_LIBRARIES
        diophantus
)

dio_test_case(ValidatorTest
    TEST_SOURCES
        ValidatorTest.cpp
    TEST_LIBRARIES
        diophantus
)

dio_test_case(SolverTest
    TEST_SOURCES
        SolverTest.cpp
    TEST_LIBRARIES
        diophantus
)
