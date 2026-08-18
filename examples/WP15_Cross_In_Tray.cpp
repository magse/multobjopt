/**
 * @file WP15_Cross_In_Tray.cpp
 * @brief Wikipedia Cross-in-tray single-objective benchmark.
 *
 * This example transcribes the two-dimensional Cross-in-tray function, its
 * `[-10, 10]` search square, and one of the four published global minimizers.
 * Its oscillatory exponential landscape is a compact demonstration of a
 * bounded, highly multimodal objective.
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
        .add_objective("cross_in_tray", objective_sense::minimize, [](scalar_view values) {
            const scalar x = values[0];
            const scalar y = values[1];
            const scalar radius = std::sqrt(x * x + y * y);
            const scalar exponential =
                std::exp(std::abs(100.0 - radius / std::numbers::pi_v<scalar>));
            const scalar oscillation = std::sin(x) * std::sin(y) * exponential;
            return -0.0001 * std::pow(std::abs(oscillation) + 1.0, 0.1);
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{1.34941, -1.34941}, {-2.06261}, 1.0e-5};
    return run_benchmark("WP15 Cross-in-tray", problem_definition, configuration);
}
