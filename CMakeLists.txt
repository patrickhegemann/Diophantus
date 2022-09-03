cmake_minimum_required(VERSION 3.16)

project(Diophantus
    VERSION 1.0
    DESCRIPTION "Linear Diophantine Equation System Solver"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#-----------------------------------------
# Fetch content
include(FetchContent)

# GoogleTest
FetchContent_Declare(
    googletest
    URL "https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip"
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# argparse
FetchContent_Declare(
    argparse
    GIT_REPOSITORY  "https://github.com/p-ranav/argparse.git"
    GIT_TAG         40a3888f151256bba95c45860af381b2b73752e8
)
FetchContent_MakeAvailable(argparse)

# -----------------------------------------
# CMake includes
list(APPEND CMAKE_PREFIX_PATH "etc/cmake")
include(etc/cmake/Util.cmake)
include(GoogleTest)

# -----------------------------------------
# Compiler and linker options
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-fconcepts)
endif ()

add_compile_options(-Wall -Wextra -Wpedantic)
# add_compile_options(-Weverything)
# add_compile_options(-v)

# Release Flags
add_compile_options(-O3)

# Debug Flags
# add_compile_options(-g)

# add_link_options(-v)

# -----------------------------------------
# Find packages
find_package(GMP REQUIRED)
find_package(Boost 1.71.0 REQUIRED COMPONENTS log)

# -----------------------------------------
# Subdirectories
add_subdirectory(src)
