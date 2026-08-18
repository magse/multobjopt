/**
 * @file WP06_Goldstein_Price.cpp
 * @brief Two-dimensional Goldstein-Price benchmark from Wikipedia's "Test
 * functions for optimization" page.
 *
 * @details This example transcribes the page's product of two polynomial
 * factors over the published square domain `[-2, 2]^2`. The known global
 * minimum is `f(0, -1) = 3`.
 *
 * @see https://en.wikipedia.org/wiki/Test_functions_for_optimization
 */

#include "wp_example_support.hpp"

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", -2.0, 2.0)
        .add_parameter("y", -2.0, 2.0)
        .add_objective("goldstein_price", objective_sense::minimize, [](scalar_view values) {
            const scalar x = values[0];
            const scalar y = values[1];
            const scalar first_sum = x + y + 1.0;
            const scalar second_difference = 2.0 * x - 3.0 * y;
            const scalar first_polynomial =
                19.0 - 14.0 * x + 3.0 * x * x - 14.0 * y + 6.0 * x * y + 3.0 * y * y;
            const scalar second_polynomial =
                18.0 - 32.0 * x + 12.0 * x * x + 48.0 * y - 36.0 * x * y + 27.0 * y * y;

            return (1.0 + first_sum * first_sum * first_polynomial) *
                   (30.0 + second_difference * second_difference * second_polynomial);
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{0.0, -1.0}, {3.0}};
    return run_benchmark("Goldstein-Price function", problem_definition, configuration);
}
