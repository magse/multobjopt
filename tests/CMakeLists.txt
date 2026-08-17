set(multobjopt_test_names
    01_test_problem_validation
    02_test_options_validation
    03_test_normalize_design
    04_test_evaluation_callbacks
    05_test_restrictions_and_validator
    06_test_objective_directions
    07_test_dominance
    08_test_algorithm_selection
    09_test_simulated_annealing
    10_test_genetic_algorithm
    11_test_box_method
    12_test_gradient_descent
    13_test_multiobjective_front
    14_test_evaluation_budget
    15_test_seed_determinism
    16_test_fixed_parameters
    17_test_twenty_five_dimensions
    18_test_nonfinite_values
    19_test_initial_guess_and_result
    20_test_enum_strings
    21_test_numerical_edge_cases
    22_test_tiny_budgets
    23_test_box_feasibility_search
    24_test_validity_scale_and_status
    26_test_types_header
    27_test_problem_header
    28_test_options_header
    29_test_result_header
    30_test_evaluation_header
    31_test_optimize_header
    32_test_version_header
)

set(multobjopt_algorithm_tests
    09_test_simulated_annealing
    10_test_genetic_algorithm
    11_test_box_method
    12_test_gradient_descent
    13_test_multiobjective_front
    14_test_evaluation_budget
    15_test_seed_determinism
    16_test_fixed_parameters
    17_test_twenty_five_dimensions
    19_test_initial_guess_and_result
    21_test_numerical_edge_cases
    22_test_tiny_budgets
    23_test_box_feasibility_search
    24_test_validity_scale_and_status
)

foreach(test_name IN LISTS multobjopt_test_names)
    set(target_name "multobjopt_${test_name}")
    add_executable(${target_name} "${test_name}.cpp")
    target_link_libraries(${target_name} PRIVATE multobjopt::multobjopt)
    target_compile_features(${target_name} PRIVATE cxx_std_20)

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_test(NAME ${test_name} COMMAND ${target_name})
    if(test_name IN_LIST multobjopt_algorithm_tests)
        set_tests_properties(
            ${test_name}
            PROPERTIES LABELS "algorithm;integration" TIMEOUT 15
        )
    else()
        set_tests_properties(
            ${test_name}
            PROPERTIES LABELS "unit" TIMEOUT 5
        )
    endif()
endforeach()

add_test(
    NAME 25_test_install_package
    COMMAND
        ${CMAKE_COMMAND}
        -Dmultobjopt_source_dir=${PROJECT_SOURCE_DIR}
        -Dmultobjopt_binary_dir=${PROJECT_BINARY_DIR}
        -Dmultobjopt_test_config=$<CONFIG>
        -P ${CMAKE_CURRENT_SOURCE_DIR}/25_test_install_package.cmake
)
set_tests_properties(
    25_test_install_package
    PROPERTIES LABELS "integration;package" TIMEOUT 30
)

if(Git_FOUND)
    add_test(
        NAME 33_test_git_version_header
        COMMAND
            ${CMAKE_COMMAND}
            -Dmultobjopt_project_source_dir=${PROJECT_SOURCE_DIR}
            -Dmultobjopt_project_binary_dir=${PROJECT_BINARY_DIR}
            -Dmultobjopt_git_executable=${GIT_EXECUTABLE}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/33_test_git_version_header.cmake
    )
    set_tests_properties(
        33_test_git_version_header
        PROPERTIES LABELS "integration;version" TIMEOUT 10
    )
endif()
