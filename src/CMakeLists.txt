cmake_minimum_required(VERSION 3.10)
project(moreward_h_tests C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(unit_tests)
target_compile_definitions(unit_tests PRIVATE UNIT_TESTS)
target_sources(unit_tests PRIVATE mg.c tests.c)

add_executable(e2e_tests)
target_sources(e2e_tests PRIVATE tests.c)
