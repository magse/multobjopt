/**
 * @file WP13_Three_Hump_Camel.cpp
 * @brief Two-dimensional Three-hump camel benchmark from Wikipedia's "Test
 * functions for optimization" page.
 *
 * @details This example transcribes the page's sixth-degree polynomial and
 * uses its published square search domain `[-5, 5]^2`. The known global
 * minimum is `f(0, 0) = 0`.
 *
 * @see https://en.wikipedia.org/wiki/Test_functions_for_optimization
 */

#include "wp_example_support.hpp"

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", -5.0, 5.0)
        .add_parameter("y", -5.0, 5.0)
        .add_objective("three_hump_camel", objective_sense::minimize, [](scalar_view values) {
            const scalar x = values[0];
            const scalar y = values[1];
            const scalar x_squared = x * x;
            const scalar x_fourth = x_squared * x_squared;
            const scalar x_sixth = x_fourth * x_squared;
            return 2.0 * x_squared - 1.05 * x_fourth + x_sixth / 6.0 + x * y + y * y;
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{0.0, 0.0}, {0.0}};
    return run_benchmark("Three-hump camel function", problem_definition, configuration);
}
