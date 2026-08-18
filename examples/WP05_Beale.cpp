/**
 * @file WP05_Beale.cpp
 * @brief Two-dimensional Beale benchmark from Wikipedia's "Test functions for
 * optimization" page.
 *
 * @details The example uses the page's two-variable polynomial and the
 * published search domain `[-4.5, 4.5]` for both parameters. Its known global
 * minimum is `f(3, 0.5) = 0`.
 *
 * @see https://en.wikipedia.org/wiki/Test_functions_for_optimization
 */

#include "wp_example_support.hpp"

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", -4.5, 4.5)
        .add_parameter("y", -4.5, 4.5)
        .add_objective("beale", objective_sense::minimize, [](scalar_view values) {
            const scalar x = values[0];
            const scalar y = values[1];
            const scalar y_squared = y * y;
            const scalar first_term = 1.5 - x + x * y;
            const scalar second_term = 2.25 - x + x * y_squared;
            const scalar third_term = 2.625 - x + x * y_squared * y;

            return first_term * first_term + second_term * second_term + third_term * third_term;
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{3.0, 0.5}, {0.0}};
    return run_benchmark("Beale function", problem_definition, configuration);
}
