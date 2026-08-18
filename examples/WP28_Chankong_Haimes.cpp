/**
 * @file WP28_Chankong_Haimes.cpp
 * @brief Wikipedia constrained Chankong-Haimes multi-objective benchmark.
 *
 * Both objectives are minimized on `[-20, 20]^2`. The page constraints
 * `x^2+y^2 <= 225` and `x-3y+10 <= 0` become the non-negative feasibility
 * margins `225-x^2-y^2 >= 0` and `3y-x-10 >= 0`.
 */

#include "wp_example_support.hpp"

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", -20.0, 20.0)
        .add_parameter("y", -20.0, 20.0)
        .add_objective("f1", objective_sense::minimize,
                       [](scalar_view values) {
                           const scalar shifted_x = values[0] - 2.0;
                           const scalar shifted_y = values[1] - 1.0;
                           return 2.0 + shifted_x * shifted_x + shifted_y * shifted_y;
                       })
        .add_objective("f2", objective_sense::minimize,
                       [](scalar_view values) {
                           const scalar shifted_y = values[1] - 1.0;
                           return 9.0 * values[0] - shifted_y * shifted_y;
                       })
        // x^2+y^2 <= 225  ->  225-x^2-y^2 >= 0.
        .add_restriction("inside_radius_15",
                         [](scalar_view values) {
                             return 225.0 - values[0] * values[0] - values[1] * values[1];
                         })
        // x-3y+10 <= 0  ->  3y-x-10 >= 0.
        .add_restriction("linear_half_plane",
                         [](scalar_view values) { return 3.0 * values[1] - values[0] - 10.0; });

    benchmark_configuration configuration;
    configuration.initial_guess = {0.0, 4.0};
    return run_benchmark("WP28 Chankong-Haimes", problem_definition, configuration);
}
