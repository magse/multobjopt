/**
 * @file WP19_Schaffer_N2.cpp
 * @brief Wikipedia Schaffer N.2 single-objective benchmark.
 *
 * This is the scalar two-variable Schaffer N.2 landscape from the page's
 * single-objective table, not the later piecewise multi-objective benchmark
 * carrying the same number. Its exact global minimum is at the origin.
 */

#include "wp_example_support.hpp"

#include <cmath>

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", -100.0, 100.0)
        .add_parameter("y", -100.0, 100.0)
        .add_objective("schaffer_n2", objective_sense::minimize, [](scalar_view values) {
            const scalar x_squared = values[0] * values[0];
            const scalar y_squared = values[1] * values[1];
            const scalar sine = std::sin(x_squared - y_squared);
            const scalar denominator = 1.0 + 0.001 * (x_squared + y_squared);
            return 0.5 + (sine * sine - 0.5) / (denominator * denominator);
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{0.0, 0.0}, {0.0}, 1.0e-12};
    return run_benchmark("WP19 Schaffer N.2 (single-objective)", problem_definition, configuration);
}
