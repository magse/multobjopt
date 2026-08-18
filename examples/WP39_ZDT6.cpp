#include "wp_example_support.hpp"

#include <cmath>
#include <cstddef>
#include <numbers>
#include <string>

/**
 * @file WP39_ZDT6.cpp
 * @brief Ten-variable nonuniform ZDT6 multiobjective benchmark.
 *
 * Every variable lies in `[0, 1]`. The first minimized objective combines an
 * exponential with a sixth-power sine term, while the second uses the catalog's
 * fourth-root tail average and quadratic shape function.
 */

namespace {

constexpr std::size_t benchmark_dimension = 10;

} // namespace

int main() {
    using namespace multobjopt;

    problem problem_definition;
    for (std::size_t index = 0; index < benchmark_dimension; ++index) {
        problem_definition.add_parameter("x_" + std::to_string(index + 1), 0.0, 1.0);
    }

    problem_definition
        .add_objective("f_1", objective_sense::minimize,
                       [](scalar_view values) {
                           const scalar sine =
                               std::sin(6.0 * std::numbers::pi_v<scalar> * values[0]);
                           return 1.0 - std::exp(-4.0 * values[0]) * std::pow(sine, 6.0);
                       })
        .add_objective("f_2", objective_sense::minimize, [](scalar_view values) {
            const scalar sine = std::sin(6.0 * std::numbers::pi_v<scalar> * values[0]);
            const scalar f_1 = 1.0 - std::exp(-4.0 * values[0]) * std::pow(sine, 6.0);

            scalar tail_sum = 0.0;
            for (std::size_t index = 1; index < values.size(); ++index) {
                tail_sum += values[index];
            }
            const scalar tail_average = tail_sum / static_cast<scalar>(benchmark_dimension - 1);
            const scalar g = 1.0 + 9.0 * std::pow(tail_average, 0.25);
            const scalar ratio = f_1 / g;
            return g * (1.0 - ratio * ratio);
        });

    return multobjopt_examples::run_benchmark("ZDT6", problem_definition);
}
