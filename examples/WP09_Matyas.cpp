/**
 * @file WP09_Matyas.cpp
 * @brief Two-dimensional Matyas benchmark from Wikipedia's "Test functions for
 * optimization" page.
 *
 * @details The example transcribes the page's coupled quadratic function and
 * uses its published square search domain `[-10, 10]^2`. The known global
 * minimum is `f(0, 0) = 0`.
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
        .add_objective("matyas", objective_sense::minimize, [](scalar_view values) {
            const scalar x = values[0];
            const scalar y = values[1];
            return 0.26 * (x * x + y * y) - 0.48 * x * y;
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{0.0, 0.0}, {0.0}};
    return run_benchmark("Matyas function", problem_definition, configuration);
}
