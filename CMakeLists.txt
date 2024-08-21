cmake_minimum_required(VERSION 3.10)
project(tests C)

# Set the C standard to C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(dev_src
    src/alloc.c
    src/buffer.c
    src/list.c
    src/dict.c
    src/strbuffer.c
    src/log.c
)

set(test_src
    tests/tests.c
    tests/test_list.c
    tests/test_dict.c
    tests/test_strbuffer.c
    tests/test_create_destroy.c
)

add_executable(unit_tests)
target_compile_definitions(unit_tests PRIVATE UNIT_TESTS)
target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_sources(unit_tests PRIVATE
    ${dev_src}
    ${test_src}
)

add_executable(e2e_tests)
target_include_directories(e2e_tests PRIVATE ${CMAKE_SOURCE_DIR})
target_sources(e2e_tests PRIVATE
    ${test_src}
)

add_executable(sample)
target_include_directories(sample PRIVATE ${CMAKE_SOURCE_DIR})
target_sources(sample PRIVATE
    sample/sample.c
)
