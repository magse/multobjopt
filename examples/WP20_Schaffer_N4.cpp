/**
 * @file WP20_Schaffer_N4.cpp
 * @brief Wikipedia Schaffer N.4 single-objective benchmark.
 *
 * The Schaffer N.4 function is evaluated on the catalog square
 * `[-100, 100]^2`. One of its four symmetric published minimizers is used for
 * the independent reference calculation.
 */

#include "wp_example_support.hpp"

#include <cmath>

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", -100.0, 100.0)
        .add_parameter("y", -100.0, 100.0)
        .add_objective("schaffer_n4", objective_sense::minimize, [](scalar_view values) {
            const scalar x_squared = values[0] * values[0];
            const scalar y_squared = values[1] * values[1];
            const scalar cosine = std::cos(std::sin(std::abs(x_squared - y_squared)));
            const scalar denominator = 1.0 + 0.001 * (x_squared + y_squared);
            return 0.5 + (cosine * cosine - 0.5) / (denominator * denominator);
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{0.0, 1.25313}, {0.292579}, 1.0e-5};
    return run_benchmark("WP20 Schaffer N.4", problem_definition, configuration);
}
