/**
 * @file WP23_Rosenbrock_Disk.cpp
 * @brief Wikipedia Rosenbrock function constrained to a disk.
 *
 * The catalog constraint `x^2 + y^2 <= 2` is represented as the non-negative
 * feasibility margin `2 - x^2 - y^2 >= 0`. The published minimizer `(1, 1)`
 * lies exactly on that boundary.
 */

#include "wp_example_support.hpp"

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", -1.5, 1.5)
        .add_parameter("y", -1.5, 1.5)
        .add_objective("rosenbrock", objective_sense::minimize,
                       [](scalar_view values) {
                           const scalar x = values[0];
                           const scalar y = values[1];
                           const scalar first = 1.0 - x;
                           const scalar second = y - x * x;
                           return first * first + 100.0 * second * second;
                       })
        // x^2 + y^2 <= 2  ->  2 - x^2 - y^2 >= 0.
        .add_restriction("inside_disk", [](scalar_view values) {
            return 2.0 - values[0] * values[0] - values[1] * values[1];
        });

    benchmark_configuration configuration;
    configuration.initial_guess = {0.0, 0.0};
    configuration.reference = reference_design{{1.0, 1.0}, {0.0}, 1.0e-12};
    return run_benchmark("WP23 Rosenbrock constrained to a disk", problem_definition,
                         configuration);
}
