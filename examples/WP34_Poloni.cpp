#include "wp_example_support.hpp"

#include <cmath>
#include <numbers>

/**
 * @file WP34_Poloni.cpp
 * @brief Poloni two-objective trigonometric benchmark.
 *
 * This transcription minimizes Poloni's residual objective and the quadratic
 * objective centered at `(-3, -1)`. Both design variables retain the catalog
 * domain `[-pi, pi]`; the constants A1 and A2 and the corresponding B1 and B2
 * expressions are evaluated directly from the published formula.
 */

int main() {
    using namespace multobjopt;

    constexpr scalar pi = std::numbers::pi_v<scalar>;

    problem problem_definition;
    problem_definition.add_parameter("x", -pi, pi)
        .add_parameter("y", -pi, pi)
        .add_objective(
            "f_1", objective_sense::minimize,
            [](scalar_view values) {
                const scalar x = values[0];
                const scalar y = values[1];
                const scalar a_1 =
                    0.5 * std::sin(1.0) - 2.0 * std::cos(1.0) + std::sin(2.0) - 1.5 * std::cos(2.0);
                const scalar a_2 =
                    1.5 * std::sin(1.0) - std::cos(1.0) + 2.0 * std::sin(2.0) - 0.5 * std::cos(2.0);
                const scalar b_1 =
                    0.5 * std::sin(x) - 2.0 * std::cos(x) + std::sin(y) - 1.5 * std::cos(y);
                const scalar b_2 =
                    1.5 * std::sin(x) - std::cos(x) + 2.0 * std::sin(y) - 0.5 * std::cos(y);
                const scalar first_residual = a_1 - b_1;
                const scalar second_residual = a_2 - b_2;
                return 1.0 + first_residual * first_residual + second_residual * second_residual;
            })
        .add_objective("f_2", objective_sense::minimize, [](scalar_view values) {
            const scalar first_difference = values[0] + 3.0;
            const scalar second_difference = values[1] + 1.0;
            return first_difference * first_difference + second_difference * second_difference;
        });

    return multobjopt_examples::run_benchmark("Poloni", problem_definition);
}
