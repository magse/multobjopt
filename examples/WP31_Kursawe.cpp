#include "wp_example_support.hpp"

#include <cmath>
#include <cstddef>
#include <string>

/**
 * @file WP31_Kursawe.cpp
 * @brief Three-variable Kursawe multiobjective benchmark.
 *
 * The first minimization objective couples adjacent variables through two
 * negative exponential terms. The second combines fractional absolute powers
 * and oscillatory cubic sine terms. All three variables use the catalog domain
 * `[-5, 5]`.
 */

namespace {

constexpr std::size_t benchmark_dimension = 3;

} // namespace

int main() {
    using namespace multobjopt;

    problem problem_definition;
    for (std::size_t index = 0; index < benchmark_dimension; ++index) {
        problem_definition.add_parameter("x_" + std::to_string(index + 1), -5.0, 5.0);
    }

    problem_definition
        .add_objective("f_1", objective_sense::minimize,
                       [](scalar_view values) {
                           scalar result = 0.0;
                           for (std::size_t index = 0; index + 1 < values.size(); ++index) {
                               const scalar radius =
                                   std::sqrt(values[index] * values[index] +
                                             values[index + 1] * values[index + 1]);
                               result += -10.0 * std::exp(-0.2 * radius);
                           }
                           return result;
                       })
        .add_objective("f_2", objective_sense::minimize, [](scalar_view values) {
            scalar result = 0.0;
            for (const scalar value : values) {
                result += std::pow(std::abs(value), 0.8) + 5.0 * std::sin(value * value * value);
            }
            return result;
        });

    return multobjopt_examples::run_benchmark("Kursawe", problem_definition);
}
