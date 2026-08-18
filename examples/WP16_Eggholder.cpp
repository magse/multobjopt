/**
 * @file WP16_Eggholder.cpp
 * @brief Wikipedia Eggholder single-objective benchmark.
 *
 * The Eggholder function combines nested absolute values, square roots, and
 * trigonometric terms over the large square `[-512, 512]^2`. The published
 * boundary minimizer is evaluated independently by the common example runner.
 */

#include "wp_example_support.hpp"

#include <cmath>

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", -512.0, 512.0)
        .add_parameter("y", -512.0, 512.0)
        .add_objective("eggholder", objective_sense::minimize, [](scalar_view values) {
            const scalar x = values[0];
            const scalar shifted_y = values[1] + 47.0;
            return -shifted_y * std::sin(std::sqrt(std::abs(x / 2.0 + shifted_y))) -
                   x * std::sin(std::sqrt(std::abs(x - shifted_y)));
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{512.0, 404.2319}, {-959.6407}, 1.0e-5};
    return run_benchmark("WP16 Eggholder", problem_definition, configuration);
}
