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
add_multobjopt_example(
    multobjopt_example_05_optional_reporting
    05_optional_reporting.cpp
)

# Keep the Wikipedia benchmark numbering visible in both the source names and
# executable names. The explicit list makes additions and catalog order easy to
# review while the loop avoids repeating identical target boilerplate.
set(
    multobjopt_wikipedia_benchmark_sources
    WP01_Rastrigin.cpp
    WP02_Ackley.cpp
    WP03_Sphere.cpp
    WP04_Rosenbrock.cpp
    WP05_Beale.cpp
    WP06_Goldstein_Price.cpp
    WP07_Booth.cpp
    WP08_Bukin_N6.cpp
    WP09_Matyas.cpp
    WP10_Levi_N13.cpp
    WP11_Griewank.cpp
    WP12_Himmelblau.cpp
    WP13_Three_Hump_Camel.cpp
    WP14_Easom.cpp
    WP15_Cross_In_Tray.cpp
    WP16_Eggholder.cpp
    WP17_Holder_Table.cpp
    WP18_McCormick.cpp
    WP19_Schaffer_N2.cpp
    WP20_Schaffer_N4.cpp
    WP21_Styblinski_Tang.cpp
    WP22_Shekel.cpp
    WP23_Rosenbrock_Disk.cpp
    WP24_Mishra_Bird_Constrained.cpp
    WP25_Townsend_Modified.cpp
    WP26_Keane_Bump.cpp
    WP27_Binh_Korn.cpp
    WP28_Chankong_Haimes.cpp
    WP29_Fonseca_Fleming.cpp
    WP30_Test_Function_4.cpp
    WP31_Kursawe.cpp
    WP32_Schaffer_N1_Multiobjective.cpp
    WP33_Schaffer_N2_Multiobjective.cpp
    WP34_Poloni.cpp
    WP35_ZDT1.cpp
    WP36_ZDT2.cpp
    WP37_ZDT3.cpp
    WP38_ZDT4.cpp
    WP39_ZDT6.cpp
    WP40_Osyczka_Kundu.cpp
    WP41_CTP1.cpp
    WP42_Constr_Ex.cpp
    WP43_Viennet.cpp
)

foreach(benchmark_source IN LISTS multobjopt_wikipedia_benchmark_sources)
    get_filename_component(benchmark_stem "${benchmark_source}" NAME_WE)
    string(TOLOWER "${benchmark_stem}" benchmark_target_suffix)
    add_multobjopt_example(
        "multobjopt_example_${benchmark_target_suffix}"
        "${benchmark_source}"
    )
    if(MULTOBJOPT_BUILD_TESTS)
        set_tests_properties(
            "multobjopt_example_${benchmark_target_suffix}"
            PROPERTIES LABELS "benchmark;example;integration"
        )
    endif()
endforeach()
