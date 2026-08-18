/**
 * @file WP12_Himmelblau.cpp
 * @brief Two-dimensional Himmelblau benchmark from Wikipedia's "Test functions
 * for optimization" page.
 *
 * @details The example uses the page's quartic formula and published square
 * search domain `[-5, 5]^2`. Four global minimizers are listed there; the exact
 * point `(3, 2)` is used for the reference check and gives `f(3, 2) = 0`.
 * The other published minimizers are approximately `(-2.805118, 3.131312)`,
 * `(-3.779310, -3.283186)`, and `(3.584428, -1.848126)`.
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
        .add_objective("himmelblau", objective_sense::minimize, [](scalar_view values) {
            const scalar x = values[0];
            const scalar y = values[1];
            const scalar first_term = x * x + y - 11.0;
            const scalar second_term = x + y * y - 7.0;
            return first_term * first_term + second_term * second_term;
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{3.0, 2.0}, {0.0}};
    return run_benchmark("Himmelblau's function", problem_definition, configuration);
}
