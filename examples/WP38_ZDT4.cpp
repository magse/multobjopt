#include "wp_example_support.hpp"

#include <cmath>
#include <cstddef>
#include <numbers>
#include <string>

/**
 * @file WP38_ZDT4.cpp
 * @brief Ten-variable multimodal ZDT4 benchmark.
 *
 * The first variable uses `[0, 1]`, while the remaining nine use `[-5, 5]`.
 * The implementation retains the catalog's Rastrigin-like scale expression
 * `g = 91 + sum(x_i^2 - 10 cos(4 pi x_i))` and minimizes both ZDT4 objectives.
 */

namespace {

constexpr std::size_t benchmark_dimension = 10;

} // namespace

int main() {
    using namespace multobjopt;

    problem problem_definition;
    problem_definition.add_parameter("x_1", 0.0, 1.0);
    for (std::size_t index = 1; index < benchmark_dimension; ++index) {
        problem_definition.add_parameter("x_" + std::to_string(index + 1), -5.0, 5.0);
    }

    problem_definition
        .add_objective("f_1", objective_sense::minimize,
                       [](scalar_view values) { return values[0]; })
        .add_objective("f_2", objective_sense::minimize, [](scalar_view values) {
            scalar g = 91.0;
            for (std::size_t index = 1; index < values.size(); ++index) {
                const scalar value = values[index];
                g += value * value - 10.0 * std::cos(4.0 * std::numbers::pi_v<scalar> * value);
            }
            return g * (1.0 - std::sqrt(values[0] / g));
        });

    return multobjopt_examples::run_benchmark("ZDT4", problem_definition);
}
