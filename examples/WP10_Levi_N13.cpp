/**
 * @file WP10_Levi_N13.cpp
 * @brief Two-dimensional Levi function N.13 benchmark from Wikipedia's "Test
 * functions for optimization" page.
 *
 * @details This example follows the page's three-term trigonometric formula on
 * its published square search domain `[-10, 10]^2`. The known global minimum
 * is `f(1, 1) = 0`.
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
    problem_definition.add_parameter("x", -10.0, 10.0)
        .add_parameter("y", -10.0, 10.0)
        .add_objective("levi_n13", objective_sense::minimize, [](scalar_view values) {
            constexpr scalar pi = std::numbers::pi_v<scalar>;
            const scalar x = values[0];
            const scalar y = values[1];
            const scalar sin_three_pi_x = std::sin(3.0 * pi * x);
            const scalar sin_three_pi_y = std::sin(3.0 * pi * y);
            const scalar sin_two_pi_y = std::sin(2.0 * pi * y);
            const scalar x_offset = x - 1.0;
            const scalar y_offset = y - 1.0;

            return sin_three_pi_x * sin_three_pi_x +
                   x_offset * x_offset * (1.0 + sin_three_pi_y * sin_three_pi_y) +
                   y_offset * y_offset * (1.0 + sin_two_pi_y * sin_two_pi_y);
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{1.0, 1.0}, {0.0}};
    return run_benchmark("Levi function N.13", problem_definition, configuration);
}
