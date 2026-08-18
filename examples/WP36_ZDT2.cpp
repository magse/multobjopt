#include "wp_example_support.hpp"

#include <cstddef>
#include <string>

/**
 * @file WP36_ZDT2.cpp
 * @brief Twenty-five-variable adaptation of the ZDT2 benchmark.
 *
 * Wikipedia defines ZDT2 in 30 dimensions over the unit hypercube. Because
 * multobjopt's public problem contract caps the dimension at 25, this example
 * explicitly sets `n = 25` and uses
 * `g = 1 + 9 * sum(x_2..x_n) / (n - 1)`. The quadratic ZDT2 shape function and
 * both minimization senses are preserved exactly.
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
            return g * (1.0 - ratio * ratio);
        });

    return multobjopt_examples::run_benchmark("ZDT2 (25D adaptation)", problem_definition);
}
