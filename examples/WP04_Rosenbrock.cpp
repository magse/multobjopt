/**
 * @file WP04_Rosenbrock.cpp
 * @brief Five-dimensional Rosenbrock benchmark from Wikipedia's "Test
 * functions for optimization" page.
 *
 * @details The published chained-valley function is scalable and has an
 * unbounded domain. This example chooses five dimensions and the finite working
 * domain `[-5, 5]^5`. Its known global minimum is
 * `f(1, 1, 1, 1, 1) = 0`.
 *
 * @see https://en.wikipedia.org/wiki/Test_functions_for_optimization
 */

#include "wp_example_support.hpp"

#include <cstddef>

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x_1", -5.0, 5.0)
        .add_parameter("x_2", -5.0, 5.0)
        .add_parameter("x_3", -5.0, 5.0)
        .add_parameter("x_4", -5.0, 5.0)
        .add_parameter("x_5", -5.0, 5.0)
        .add_objective("rosenbrock", objective_sense::minimize, [](scalar_view values) {
            scalar objective = 0.0;
            for (std::size_t index = 0; index + 1 < values.size(); ++index) {
                const scalar valley_error = values[index + 1] - values[index] * values[index];
                const scalar location_error = 1.0 - values[index];
                objective += 100.0 * valley_error * valley_error + location_error * location_error;
            }
            return objective;
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{1.0, 1.0, 1.0, 1.0, 1.0}, {0.0}};
    return run_benchmark("Rosenbrock function", problem_definition, configuration);
}
