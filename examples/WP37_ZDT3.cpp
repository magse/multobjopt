#include "wp_example_support.hpp"

#include <cmath>
#include <cstddef>
#include <numbers>
#include <string>

/**
 * @file WP37_ZDT3.cpp
 * @brief Twenty-five-variable adaptation of the disconnected ZDT3 benchmark.
 *
 * The catalog's 30-dimensional unit hypercube is adapted to multobjopt's
 * 25-parameter maximum. With `n = 25`, the scale term is explicitly
 * `g = 1 + 9 * sum(x_2..x_n) / (n - 1)`. The square-root and oscillatory
 * portions of the ZDT3 shape function remain unchanged, and both objectives
 * are minimized.
 */

namespace {

constexpr std::size_t benchmark_dimension = 25;

} // namespace

int main() {
    using namespace multobjopt;

    problem problem_definition;
    for (std::size_t index = 0; index < benchmark_dimension; ++index) {
        problem_definition.add_parameter("x_" + std::to_string(index + 1), 0.0, 1.0);
    }

    problem_definition
        .add_objective("f_1", objective_sense::minimize,
                       [](scalar_view values) { return values[0]; })
        .add_objective("f_2", objective_sense::minimize, [](scalar_view values) {
            scalar tail_sum = 0.0;
            for (std::size_t index = 1; index < values.size(); ++index) {
                tail_sum += values[index];
            }
            const scalar g = 1.0 + 9.0 * tail_sum / static_cast<scalar>(benchmark_dimension - 1);
            const scalar ratio = values[0] / g;
            const scalar h = 1.0 - std::sqrt(ratio) -
                             ratio * std::sin(10.0 * std::numbers::pi_v<scalar> * values[0]);
            return g * h;
        });

    return multobjopt_examples::run_benchmark("ZDT3 (25D adaptation)", problem_definition);
}
