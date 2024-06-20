cmake_minimum_required(VERSION 3.15)
project(hipfish LANGUAGES C)
set(CMAKE_C_STANDARD 90)  # Set the default C standard to C89 globally. C90 corresponds to C89 in cmake. Can be overridden per target.
set(CMAKE_C_STANDARD_REQUIRED on)


set(compiler_warnings "-Wall" "-Wextra")

set(src_impl
    ../src/_a.h
    ../src/_b.h
    ../src/_e.h
    ../src/mg.h
    ../src/mg_alloc.c
    ../src/mg_buffer.c
    ../src/mg_list.c
)

# these test cases are shared between unit and e2e tests
set(src_cases
    ./cases/t_smoke.c
    ./cases/t_basic.c
    ./cases/t_list.c
)

# unit tests use implementation files, not the resulting 'moreward.h'
set(src_unit
    ${src_impl}
    ./unit/test_unit.c
    ${src_cases}
)

# e2e tests 'moreward.h' directly
set(src_e2e
    ./e2e/test_e2e.c
    ${src_cases}
)

add_executable(test_unit)
add_executable(test_e2e)

target_compile_definitions(test_unit PRIVATE TEST_UNIT=1)

target_include_directories(test_unit PRIVATE ../src deps)
target_include_directories(test_e2e PRIVATE ../ deps)

target_compile_options(test_unit PRIVATE ${compiler_warnings})
target_compile_options(test_e2e PRIVATE ${compiler_warnings})

target_sources(test_unit PRIVATE ${src_unit})
target_sources(test_e2e PRIVATE ${src_e2e})