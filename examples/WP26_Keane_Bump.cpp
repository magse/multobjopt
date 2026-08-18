/**
 * @file WP26_Keane_Bump.cpp
 * @brief Two-dimensional Wikipedia Keane bump constrained benchmark.
 *
 * For `m = 2`, the page constraints become `x*y > 0.75` and `x+y < 15`.
 * They are supplied to multobjopt as `x*y - 0.75 >= 0` and
 * `15 - (x+y) >= 0`. As with the catalog's strict parameter and constraint
 * bounds, the scalar API includes equality at zero; the published minimizer is
 * strictly feasible to the precision shown on the page.
 */

#include "wp_example_support.hpp"

#include <cmath>
#include <limits>

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x1", 0.0, 10.0)
        .add_parameter("x2", 0.0, 10.0)
        .add_objective("keane_bump", objective_sense::minimize,
                       [](scalar_view values) {
                           const scalar x = values[0];
                           const scalar y = values[1];
                           const scalar cosine_x_squared = std::cos(x) * std::cos(x);
                           const scalar cosine_y_squared = std::cos(y) * std::cos(y);
                           const scalar numerator = cosine_x_squared * cosine_x_squared +
                                                    cosine_y_squared * cosine_y_squared -
                                                    2.0 * cosine_x_squared * cosine_y_squared;
                           const scalar denominator = std::sqrt(x * x + 2.0 * y * y);
                           if (denominator == 0.0) {
                               return std::numeric_limits<scalar>::infinity();
                           }
                           return -std::abs(numerator / denominator);
                       })
        // 0.75 - x*y < 0  ->  x*y - 0.75 >= 0.
        .add_restriction("minimum_product",
                         [](scalar_view values) { return values[0] * values[1] - 0.75; })
        // x+y - 7.5*m < 0 with m=2  ->  15 - (x+y) >= 0.
        .add_restriction("maximum_sum",
                         [](scalar_view values) { return 15.0 - values[0] - values[1]; });

    benchmark_configuration configuration;
    configuration.initial_guess = {1.6, 0.5};
    configuration.reference = reference_design{{1.60025376, 0.468675907}, {-0.364979746}, 1.0e-6};
    return run_benchmark("WP26 Keane bump (2D)", problem_definition, configuration);
}
