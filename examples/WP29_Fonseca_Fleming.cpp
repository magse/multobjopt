#include "wp_example_support.hpp"

#include <cmath>
#include <cstddef>
#include <string>

/**
 * @file WP29_Fonseca_Fleming.cpp
 * @brief Three-variable Fonseca-Fleming multiobjective benchmark.
 *
 * This example transcribes the two minimization objectives and the
 * `[-4, 4]^n` search domain from Wikipedia's optimization-test-function
 * catalog. The catalog permits a general dimension; this executable uses the
 * common three-variable form, so each exponential basin is shifted by
 * `1 / sqrt(3)` in opposite directions.
 */

namespace {

constexpr std::size_t benchmark_dimension = 3;

} // namespace

int main() {
    using namespace multobjopt;

    problem problem_definition;
    for (std::size_t index = 0; index < benchmark_dimension; ++index) {
        problem_definition.add_parameter("x_" + std::to_string(index + 1), -4.0, 4.0);
    }

    problem_definition
        .add_objective("f_1", objective_sense::minimize,
                       [](scalar_view values) {
                           const scalar shift =
                               1.0 / std::sqrt(static_cast<scalar>(benchmark_dimension));
                           scalar squared_distance = 0.0;
                           for (const scalar value : values) {
                               const scalar difference = value - shift;
                               squared_distance += difference * difference;
                           }
                           return 1.0 - std::exp(-squared_distance);
                       })
        .add_objective("f_2", objective_sense::minimize, [](scalar_view values) {
            const scalar shift = 1.0 / std::sqrt(static_cast<scalar>(benchmark_dimension));
            scalar squared_distance = 0.0;
            for (const scalar value : values) {
                const scalar difference = value + shift;
                squared_distance += difference * difference;
            }
            return 1.0 - std::exp(-squared_distance);
        });

    return multobjopt_examples::run_benchmark("Fonseca-Fleming", problem_definition);
}
