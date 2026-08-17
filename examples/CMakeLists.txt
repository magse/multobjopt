if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.20)
    project(multobjopt_examples LANGUAGES CXX)

    find_package(multobjopt CONFIG REQUIRED)
endif()

function(add_multobjopt_example target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} PRIVATE multobjopt::multobjopt)
    target_compile_features(${target_name} PRIVATE cxx_std_20)
    if(MULTOBJOPT_BUILD_TESTS)
        add_test(NAME ${target_name} COMMAND ${target_name})
        set_tests_properties(
            ${target_name}
            PROPERTIES LABELS "example;integration" TIMEOUT 15
        )
    endif()
endfunction()

add_multobjopt_example(
    multobjopt_example_01_basic_quadratic
    01_basic_quadratic.cpp
)
add_multobjopt_example(
    multobjopt_example_02_constrained_box
    02_constrained_box.cpp
)
add_multobjopt_example(
    multobjopt_example_03_multiobjective_pareto
    03_multiobjective_pareto.cpp
)
add_multobjopt_example(
    multobjopt_example_04_mechatronic_discrete_auto
    04_mechatronic_discrete_auto.cpp
)
