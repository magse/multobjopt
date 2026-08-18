/**
 * @file WP08_Bukin_N6.cpp
 * @brief Two-dimensional Bukin function N.6 benchmark from Wikipedia's "Test
 * functions for optimization" page.
 *
 * @details This example uses the page's asymmetric search domain,
 * `-15 <= x <= -5` and `-3 <= y <= 3`, together with its nonsmooth square-root
 * and absolute-value formula. The known global minimum is `f(-10, 1) = 0`.
 * Its reference tolerance allows for roundoff amplified by the square root at
 * this cusp.
 *
 * @see https://en.wikipedia.org/wiki/Test_functions_for_optimization
 */

#include "wp_example_support.hpp"

#include <cmath>

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", -15.0, -5.0)
        .add_parameter("y", -3.0, 3.0)
        .add_objective("bukin_n6", objective_sense::minimize, [](scalar_view values) {
            const scalar x = values[0];
            const scalar y = values[1];
            return 100.0 * std::sqrt(std::abs(y - 0.01 * (x * x))) + 0.01 * std::abs(x + 10.0);
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{-10.0, 1.0}, {0.0}, 1.0e-5};
    return run_benchmark("Bukin function N.6", problem_definition, configuration);
}
