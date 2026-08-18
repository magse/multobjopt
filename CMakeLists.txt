cmake_minimum_required(VERSION 3.20)

project(
    multobjopt
    VERSION 0.1.2
    DESCRIPTION "Dependency-free multi-objective optimisation for bounded engineering design problems"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
find_package(Git QUIET)

option(MULTOBJOPT_BUILD_TESTS "Build the multobjopt test suite" ${PROJECT_IS_TOP_LEVEL})
option(MULTOBJOPT_BUILD_EXAMPLES "Build the multobjopt examples" ${PROJECT_IS_TOP_LEVEL})
option(MULTOBJOPT_BUILD_DOCUMENTATION "Build the multobjopt Doxygen documentation" OFF)

add_library(
    multobjopt
    # Problem construction and configuration.
    src/problem_add_objective.cpp
    src/problem_add_parameter.cpp
    src/problem_add_restriction.cpp
    src/problem_objectives.cpp
    src/problem_parameters.cpp
    src/problem_restrictions.cpp
    src/problem_set_validation.cpp
    src/problem_validate.cpp
    src/problem_validation.cpp
    src/optimizer_options_validate.cpp

    # Public evaluation operations and their shared numerical support.
    src/canonical_objective.cpp
    src/dominates.cpp
    src/equal_objectives.cpp
    src/evaluate_design.cpp
    src/evaluate_unchecked.cpp
    src/finite.cpp
    src/lexicographically_less.cpp
    src/normalize_design.cpp
    src/normalize_design_unchecked.cpp
    src/normalize_parameter.cpp
    src/rank_before.cpp
    src/require.cpp
    src/significant_improvement.cpp
    src/sortable_objective.cpp

    # Shared optimizer context, with one evaluator operation per file.
    src/evaluator_archive.cpp
    src/evaluator_archive_revision.cpp
    src/evaluator_best.cpp
    src/evaluator_can_evaluate.cpp
    src/evaluator_constructor.cpp
    src/evaluator_evaluate.cpp
    src/evaluator_evaluation_history.cpp
    src/evaluator_evaluations.cpp
    src/evaluator_update_archive.cpp
    src/finish_result.cpp
    src/free_parameter_count.cpp
    src/midpoint.cpp
    src/random_point.cpp
    src/starting_point.cpp

    # Optional completed-run report serialization.
    src/reporting_delimited.cpp
    src/reporting_format.cpp
    src/reporting_json.cpp
    src/reporting_validate.cpp
    src/write_evaluation_history.cpp
    src/write_pareto_front.cpp
    src/write_summary_json.cpp

    # Public selection, dispatch, and enum formatting operations.
    src/optimization_algorithm_to_string.cpp
    src/optimize_problem.cpp
    src/select_algorithm.cpp
    src/termination_reason_to_string.cpp

    # Optimizer entry points and focused algorithm support operations.
    src/annealing_worsening_delta.cpp
    src/box_method.cpp
    src/genetic_algorithm.cpp
    src/gradient_descent.cpp
    src/gradient_merit.cpp
    src/metric_before.cpp
    src/normalized_complex_diameter.cpp
    src/population_metrics.cpp
    src/simulated_annealing.cpp
    src/tournament_select.cpp
)
add_library(multobjopt::multobjopt ALIAS multobjopt)

set(
    multobjopt_generated_include_dir
    "${CMAKE_CURRENT_BINARY_DIR}/generated/include"
)
set(
    multobjopt_version_header
    "${multobjopt_generated_include_dir}/multobjopt/version.hpp"
)
set(multobjopt_git_executable "")
if(Git_FOUND)
    set(multobjopt_git_executable "${GIT_EXECUTABLE}")
endif()

set(
    multobjopt_version_generator_arguments
    "-Dmultobjopt_version_template=${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.hpp.in"
    "-Dmultobjopt_version_output=${multobjopt_version_header}"
    "-Dmultobjopt_source_dir=${CMAKE_CURRENT_SOURCE_DIR}"
    "-Dmultobjopt_git_executable=${multobjopt_git_executable}"
    "-Dmultobjopt_project_version=${PROJECT_VERSION}"
    "-Dmultobjopt_project_version_major=${PROJECT_VERSION_MAJOR}"
    "-Dmultobjopt_project_version_minor=${PROJECT_VERSION_MINOR}"
    "-Dmultobjopt_project_version_patch=${PROJECT_VERSION_PATCH}"
)

execute_process(
    COMMAND
        ${CMAKE_COMMAND}
        ${multobjopt_version_generator_arguments}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_version_header.cmake
    RESULT_VARIABLE multobjopt_version_generation_result
)
if(NOT multobjopt_version_generation_result EQUAL 0)
    message(FATAL_ERROR "failed to generate multobjopt/version.hpp")
endif()

add_custom_target(
    multobjopt_generate_version_header
    COMMAND
        ${CMAKE_COMMAND}
        ${multobjopt_version_generator_arguments}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_version_header.cmake
    BYPRODUCTS ${multobjopt_version_header}
    COMMENT "Refreshing multobjopt Git version header"
    VERBATIM
)
add_dependencies(multobjopt multobjopt_generate_version_header)
set_source_files_properties(${multobjopt_version_header} PROPERTIES GENERATED TRUE)
target_sources(multobjopt PRIVATE ${multobjopt_version_header})

target_compile_features(multobjopt PUBLIC cxx_std_20)
target_include_directories(
    multobjopt
    PUBLIC
        $<BUILD_INTERFACE:${multobjopt_generated_include_dir}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(
    multobjopt
    PROPERTIES
        CXX_EXTENSIONS OFF
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

if(MSVC)
    target_compile_options(multobjopt PRIVATE /W4 /permissive-)
else()
    target_compile_options(multobjopt PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(MULTOBJOPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(MULTOBJOPT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(MULTOBJOPT_BUILD_DOCUMENTATION)
    find_package(Doxygen REQUIRED)
    configure_file(Doxyfile.in Doxyfile @ONLY)
    add_custom_target(
        multobjopt_documentation ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating multobjopt API documentation"
        VERBATIM
    )
    add_dependencies(
        multobjopt_documentation
        multobjopt_generate_version_header
    )
endif()

install(
    TARGETS multobjopt
    EXPORT multobjopt_targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    FILES ${multobjopt_version_header}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/multobjopt
)

install(
    EXPORT multobjopt_targets
    FILE multobjoptTargets.cmake
    NAMESPACE multobjopt::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/multobjopt
)

configure_package_config_file(
    cmake/multobjoptConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/multobjoptConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/multobjopt
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/multobjoptConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/multobjoptConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/multobjoptConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/multobjopt
)
