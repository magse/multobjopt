/**
 * @file WP11_Griewank.cpp
 * @brief Five-dimensional Griewank benchmark from Wikipedia's "Test functions
 * for optimization" page.
 *
 * @details The published sum-and-product function is scalable and has an
 * unbounded domain. This example chooses five dimensions and the finite working
 * domain `[-600, 600]^5`. Its known global minimum is
 * `f(0, 0, 0, 0, 0) = 0`.
 *
 * @see https://en.wikipedia.org/wiki/Test_functions_for_optimization
 */

#include "wp_example_support.hpp"

#include <cmath>
#include <cstddef>

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x_1", -600.0, 600.0)
        .add_parameter("x_2", -600.0, 600.0)
        .add_parameter("x_3", -600.0, 600.0)
        .add_parameter("x_4", -600.0, 600.0)
        .add_parameter("x_5", -600.0, 600.0)
        .add_objective("griewank", objective_sense::minimize, [](scalar_view values) {
            scalar squared_sum = 0.0;
            scalar cosine_product = 1.0;
            for (std::size_t index = 0; index < values.size(); ++index) {
                const scalar value = values[index];
                squared_sum += value * value;
                cosine_product *= std::cos(value / std::sqrt(static_cast<scalar>(index + 1)));
            }
            return 1.0 + squared_sum / 4000.0 - cosine_product;
        });

    benchmark_configuration configuration;
    configuration.reference = reference_design{{0.0, 0.0, 0.0, 0.0, 0.0}, {0.0}};
    return run_benchmark("Griewank function", problem_definition, configuration);
}
