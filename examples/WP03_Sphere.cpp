/**
 * @file WP03_Sphere.cpp
 * @brief Five-dimensional Sphere benchmark from Wikipedia's "Test functions
 * for optimization" page.
 *
 * @details The published sum-of-squares function is scalable and has an
 * unbounded domain. This example chooses five dimensions and the finite working
 * domain `[-5, 5]^5`, which contains its known global minimum
 * `f(0, 0, 0, 0, 0) = 0`.
 *
 * @see https://en.wikipedia.org/wiki/Test_functions_for_optimization
 */

#include "wp_example_support.hpp"

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x_1", -5.0, 5.0)
        .add_parameter("x_2", -5.0, 5.0)
        .add_parameter("x_3", -5.0, 5.0)
        .add_parameter("x_4", -5.0, 5.0)
        .add_parameter("x_5", -5.0, 5.0)
        .add_objective("sphere", objective_sense::minimize, [](scalar_view values) {
            scalar objective = 0.0;
            for (const scalar value : values) {
                objective += value * value;
            }
            return objective;
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{0.0, 0.0, 0.0, 0.0, 0.0}, {0.0}};
    return run_benchmark("Sphere function", problem_definition, configuration);
}
