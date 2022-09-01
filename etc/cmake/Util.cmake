include(CMakeParseArguments)
enable_testing()

function(dio_test_case TEST_NAME)
    set(prefix DIO)
    set(flags )
    set(singleValues )
    set(multiValues TEST_SOURCES TEST_LIBRARIES)

    cmake_parse_arguments(
        ${prefix}
        "${flags}"
        "${singleValues}"
        "${multiValues}"
        ${ARGN}
    )

    add_executable(${TEST_NAME} ${DIO_TEST_SOURCES})
    set_target_properties(${TEST_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test"
    )
    target_link_libraries(${TEST_NAME} PRIVATE ${DIO_TEST_LIBRARIES} gtest_main)
    
    gtest_discover_tests(${TEST_NAME})
endfunction(dio_test_case)
