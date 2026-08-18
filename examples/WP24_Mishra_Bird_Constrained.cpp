/**
 * @file WP24_Mishra_Bird_Constrained.cpp
 * @brief Wikipedia constrained Mishra's Bird benchmark.
 *
 * The page's open-disk condition `(x+5)^2 + (y+5)^2 < 25` is rearranged to
 * `25 - (x+5)^2 - (y+5)^2 >= 0`. The scalar restriction API includes the zero
 * boundary, while the published minimizer lies strictly inside the disk.
 */

#include "wp_example_support.hpp"

#include <cmath>

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", -10.0, 0.0)
        .add_parameter("y", -6.5, 0.0)
        .add_objective("mishra_bird", objective_sense::minimize,
                       [](scalar_view values) {
                           const scalar x = values[0];
                           const scalar y = values[1];
                           const scalar first = 1.0 - std::cos(x);
                           const scalar second = 1.0 - std::sin(y);
                           const scalar difference = x - y;
                           return std::sin(y) * std::exp(first * first) +
                                  std::cos(x) * std::exp(second * second) + difference * difference;
                       })
        // (x+5)^2 + (y+5)^2 < 25  ->  25 - (x+5)^2 - (y+5)^2 >= 0.
        .add_restriction("inside_shifted_disk", [](scalar_view values) {
            const scalar shifted_x = values[0] + 5.0;
            const scalar shifted_y = values[1] + 5.0;
            return 25.0 - shifted_x * shifted_x - shifted_y * shifted_y;
        });

    benchmark_configuration configuration;
    configuration.initial_guess = {-4.0, -3.0};
    configuration.reference = reference_design{{-3.1302468, -1.5821422}, {-106.7645367}, 1.0e-8};
    return run_benchmark("WP24 Mishra's Bird constrained", problem_definition, configuration);
}
