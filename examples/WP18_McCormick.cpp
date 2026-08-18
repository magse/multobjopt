/**
 * @file WP18_McCormick.cpp
 * @brief Wikipedia McCormick single-objective benchmark.
 *
 * The example preserves the asymmetric catalog bounds for x and y and checks
 * the published minimizer. It also illustrates that parameter definitions may
 * use different intervals even when an objective couples both coordinates.
 */

#include "wp_example_support.hpp"

#include <cmath>

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", -1.5, 4.0)
        .add_parameter("y", -3.0, 4.0)
        .add_objective("mccormick", objective_sense::minimize, [](scalar_view values) {
            const scalar x = values[0];
            const scalar y = values[1];
            const scalar difference = x - y;
            return std::sin(x + y) + difference * difference - 1.5 * x + 2.5 * y + 1.0;
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{-0.54719, -1.54719}, {-1.9133}, 1.0e-4};
    return run_benchmark("WP18 McCormick", problem_definition, configuration);
}
