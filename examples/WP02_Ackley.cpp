/**
 * @file WP02_Ackley.cpp
 * @brief Two-dimensional Ackley benchmark from Wikipedia's "Test functions for
 * optimization" page.
 *
 * @details This example follows the page's two-variable formula and its
 * tabulated finite search domain `[-5, 5]` for both `x` and `y`. The row also
 * carries an editorial note questioning an unbounded domain; the finite table
 * bounds are used here because multobjopt parameters require finite bounds.
 * The known global minimum is `f(0, 0) = 0`.
 *
 * @see https://en.wikipedia.org/wiki/Test_functions_for_optimization
 */

#include "wp_example_support.hpp"

#include <cmath>
#include <numbers>

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", -5.0, 5.0)
        .add_parameter("y", -5.0, 5.0)
        .add_objective("ackley", objective_sense::minimize, [](scalar_view values) {
            constexpr scalar two_pi = 2.0 * std::numbers::pi_v<scalar>;
            const scalar x = values[0];
            const scalar y = values[1];
            const scalar squared_mean = 0.5 * (x * x + y * y);
            const scalar cosine_mean = 0.5 * (std::cos(two_pi * x) + std::cos(two_pi * y));

            return -20.0 * std::exp(-0.2 * std::sqrt(squared_mean)) - std::exp(cosine_mean) +
                   std::numbers::e_v<scalar> + 20.0;
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{0.0, 0.0}, {0.0}};
    return run_benchmark("Ackley function", problem_definition, configuration);
}
