#include "wp_example_support.hpp"

#include <cmath>

/**
 * @file WP43_Viennet.cpp
 * @brief Two-variable, three-objective Viennet benchmark.
 *
 * This example directly transcribes all three minimized catalog objectives.
 * Their shared radial terms and two affine quadratic expressions are evaluated
 * over the published square domain `[-3, 3]^2`.
 */

int main() {
    using namespace multobjopt;

    problem problem_definition;
    problem_definition.add_parameter("x", -3.0, 3.0)
        .add_parameter("y", -3.0, 3.0)
        .add_objective("f_1", objective_sense::minimize,
                       [](scalar_view values) {
                           const scalar radius_squared =
                               values[0] * values[0] + values[1] * values[1];
                           return 0.5 * radius_squared + std::sin(radius_squared);
                       })
        .add_objective("f_2", objective_sense::minimize,
                       [](scalar_view values) {
                           const scalar first_expression = 3.0 * values[0] - 2.0 * values[1] + 4.0;
                           const scalar second_expression = values[0] - values[1] + 1.0;
                           return first_expression * first_expression / 8.0 +
                                  second_expression * second_expression / 27.0 + 15.0;
                       })
        .add_objective("f_3", objective_sense::minimize, [](scalar_view values) {
            const scalar radius_squared = values[0] * values[0] + values[1] * values[1];
            return 1.0 / (radius_squared + 1.0) - 1.1 * std::exp(-radius_squared);
        });

    return multobjopt_examples::run_benchmark("Viennet", problem_definition);
}
