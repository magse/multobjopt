#include "wp_example_support.hpp"

#include <cmath>
#include <cstddef>
#include <string>

/**
 * @file WP35_ZDT1.cpp
 * @brief Twenty-five-variable adaptation of the ZDT1 benchmark.
 *
 * Wikipedia presents ZDT1 with 30 variables in `[0, 1]`. multobjopt accepts at
 * most 25 design parameters, so this example explicitly uses `n = 25` and the
 * dimensionally equivalent expression `g = 1 + 9 * sum(x_2..x_n) / (n - 1)`.
 * Both catalog objectives remain minimization objectives and the ZDT1 square-
 * root shape function is otherwise unchanged.
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
            return g * (1.0 - std::sqrt(ratio));
        });

    return multobjopt_examples::run_benchmark("ZDT1 (25D adaptation)", problem_definition);
}
