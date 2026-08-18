/**
 * @file WP17_Holder_Table.cpp
 * @brief Wikipedia Holder-table single-objective benchmark.
 *
 * This transcription uses the catalog formula on `[-10, 10]^2`. Symmetry
 * produces four equivalent global minima; the reference check uses the
 * published minimizer in the positive-positive quadrant.
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
        .add_objective("holder_table", objective_sense::minimize, [](scalar_view values) {
            const scalar x = values[0];
            const scalar y = values[1];
            const scalar radius = std::sqrt(x * x + y * y);
            const scalar exponential =
                std::exp(std::abs(1.0 - radius / std::numbers::pi_v<scalar>));
            return -std::abs(std::sin(x) * std::cos(y) * exponential);
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{8.05502, 9.66459}, {-19.2085}, 1.0e-5};
    return run_benchmark("WP17 Holder table", problem_definition, configuration);
}
