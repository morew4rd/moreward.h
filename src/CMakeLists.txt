cmake_minimum_required(VERSION 3.10)
project(tests C)

# Set the C standard to C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(dev_src
    mg.c
)

set(test_src
    tests.c
)

add_executable(unit_tests)
target_compile_definitions(unit_tests PRIVATE UNIT_TESTS)
target_include_directories(unit_tests PRIVATE ../deps/utest)
target_include_directories(unit_tests PRIVATE ./)
target_sources(unit_tests PRIVATE
    ${dev_src}
    ${test_src}
)

add_executable(e2e_tests)
target_compile_definitions(e2e_tests PRIVATE E2E_TESTS)
target_include_directories(e2e_tests PRIVATE ../deps/utest)
target_include_directories(e2e_tests PRIVATE ../)
target_sources(e2e_tests PRIVATE
    ${test_src}
)
