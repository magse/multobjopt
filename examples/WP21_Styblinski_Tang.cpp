/**
 * @file WP21_Styblinski_Tang.cpp
 * @brief Five-dimensional Wikipedia Styblinski-Tang benchmark.
 *
 * The catalog defines this objective for arbitrary dimension. This example
 * chooses five variables, each bounded by `[-5, 5]`, and checks the published
 * repeated-coordinate minimizer against the corresponding five-dimensional
 * objective value.
 */

#include "wp_example_support.hpp"

#include <string>

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    for (std::size_t index = 0; index < 5; ++index) {
        problem_definition.add_parameter("x" + std::to_string(index + 1), -5.0, 5.0);
    }
    problem_definition.add_objective(
        "styblinski_tang", objective_sense::minimize, [](scalar_view values) {
            scalar sum = 0.0;
            for (const scalar value : values) {
                const scalar squared = value * value;
                sum += squared * squared - 16.0 * squared + 5.0 * value;
            }
            return sum / 2.0;
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{
        {-2.903534, -2.903534, -2.903534, -2.903534, -2.903534}, {-195.830828518857}, 1.0e-10};
    return run_benchmark("WP21 Styblinski-Tang (5D)", problem_definition, configuration);
}
