/**
 * @file WP07_Booth.cpp
 * @brief Two-dimensional Booth benchmark from Wikipedia's "Test functions for
 * optimization" page.
 *
 * @details The page defines this quadratic function on the published domain
 * `[-10, 10]` for both `x` and `y`. Its known global minimum is
 * `f(1, 3) = 0`.
 *
 * @see https://en.wikipedia.org/wiki/Test_functions_for_optimization
 */

#include "wp_example_support.hpp"

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", -10.0, 10.0)
        .add_parameter("y", -10.0, 10.0)
        .add_objective("booth", objective_sense::minimize, [](scalar_view values) {
            const scalar x = values[0];
            const scalar y = values[1];
            const scalar first_term = x + 2.0 * y - 7.0;
            const scalar second_term = 2.0 * x + y - 5.0;
            return first_term * first_term + second_term * second_term;
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{1.0, 3.0}, {0.0}};
    return run_benchmark("Booth function", problem_definition, configuration);
}
