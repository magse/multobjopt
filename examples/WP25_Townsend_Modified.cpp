/**
 * @file WP25_Townsend_Modified.cpp
 * @brief Wikipedia modified Townsend constrained benchmark.
 *
 * The angular boundary follows the catalog literally with `t = atan2(x, y)`.
 * Its condition `x^2 + y^2 < radial_limit(t)` is exposed as the non-negative
 * margin `radial_limit(t) - x^2 - y^2 >= 0`; consequently the numerical API
 * includes the zero boundary of the page's strict inequality.
 */

#include "wp_example_support.hpp"

#include <cmath>

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", -2.25, 2.25)
        .add_parameter("y", -2.5, 1.75)
        .add_objective("townsend_modified", objective_sense::minimize,
                       [](scalar_view values) {
                           const scalar x = values[0];
                           const scalar y = values[1];
                           const scalar cosine = std::cos((x - 0.1) * y);
                           return -cosine * cosine - x * std::sin(3.0 * x + y);
                       })
        // x^2+y^2 < R(t)^2  ->  R(t)^2-x^2-y^2 >= 0, t=atan2(x,y).
        .add_restriction("inside_townsend_boundary", [](scalar_view values) {
            const scalar x = values[0];
            const scalar y = values[1];
            const scalar angle = std::atan2(x, y);
            const scalar first = 2.0 * std::cos(angle) - 0.5 * std::cos(2.0 * angle) -
                                 0.25 * std::cos(3.0 * angle) - 0.125 * std::cos(4.0 * angle);
            const scalar second = 2.0 * std::sin(angle);
            return first * first + second * second - x * x - y * y;
        });

    benchmark_configuration configuration;
    configuration.initial_guess = {0.0, 0.0};
    configuration.reference = reference_design{{2.0052938, 1.1944509}, {-2.0239884}, 1.0e-6};
    return run_benchmark("WP25 Townsend modified", problem_definition, configuration);
}
