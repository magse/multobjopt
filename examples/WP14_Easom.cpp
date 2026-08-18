/**
 * @file WP14_Easom.cpp
 * @brief Two-dimensional Easom benchmark from Wikipedia's "Test functions for
 * optimization" page.
 *
 * @details The example uses the page's sharply localized trigonometric formula
 * and its published square search domain `[-100, 100]^2`. The known global
 * minimum occurs at `(pi, pi)` and has the value `f(pi, pi) = -1`.
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
    problem_definition.add_parameter("x", -100.0, 100.0)
        .add_parameter("y", -100.0, 100.0)
        .add_objective("easom", objective_sense::minimize, [](scalar_view values) {
            constexpr scalar pi = std::numbers::pi_v<scalar>;
            const scalar x = values[0];
            const scalar y = values[1];
            const scalar x_offset = x - pi;
            const scalar y_offset = y - pi;
            return -std::cos(x) * std::cos(y) *
                   std::exp(-(x_offset * x_offset + y_offset * y_offset));
        });

    benchmark_configuration configuration;
    constexpr scalar pi = std::numbers::pi_v<scalar>;
    configuration.reference = reference_design{{pi, pi}, {-1.0}};
    return run_benchmark("Easom function", problem_definition, configuration);
}
