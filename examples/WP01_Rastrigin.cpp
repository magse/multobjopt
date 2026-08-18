/**
 * @file WP01_Rastrigin.cpp
 * @brief Five-dimensional Rastrigin benchmark from Wikipedia's "Test functions
 * for optimization" page.
 *
 * @details The published function is scalable; this example chooses five
 * dimensions and uses the page's search domain of `[-5.12, 5.12]` for every
 * parameter. With `A = 10`, its known global minimum is
 * `f(0, 0, 0, 0, 0) = 0`.
 *
 * @see https://en.wikipedia.org/wiki/Test_functions_for_optimization
 */

#include "wp_example_support.hpp"

#include <cmath>
#include <numbers>

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x_1", -5.12, 5.12)
        .add_parameter("x_2", -5.12, 5.12)
        .add_parameter("x_3", -5.12, 5.12)
        .add_parameter("x_4", -5.12, 5.12)
        .add_parameter("x_5", -5.12, 5.12)
        .add_objective("rastrigin", objective_sense::minimize, [](scalar_view values) {
            constexpr scalar amplitude = 10.0;
            constexpr scalar two_pi = 2.0 * std::numbers::pi_v<scalar>;

            scalar objective = amplitude * static_cast<scalar>(values.size());
            for (const scalar value : values) {
                objective += value * value - amplitude * std::cos(two_pi * value);
            }
            return objective;
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{0.0, 0.0, 0.0, 0.0, 0.0}, {0.0}};
    return run_benchmark("Rastrigin function", problem_definition, configuration);
}
