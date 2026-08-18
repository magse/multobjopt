/**
 * @file WP27_Binh_Korn.cpp
 * @brief Wikipedia constrained Binh-Korn multi-objective benchmark.
 *
 * Both catalog objectives are minimized. The constraints are transformed from
 * `(x-5)^2+y^2 <= 25` and `(x-8)^2+(y+3)^2 >= 7.7` into non-negative margins
 * `25-(x-5)^2-y^2 >= 0` and `(x-8)^2+(y+3)^2-7.7 >= 0` respectively.
 */

#include "wp_example_support.hpp"

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    problem_definition.add_parameter("x", 0.0, 5.0)
        .add_parameter("y", 0.0, 3.0)
        .add_objective("f1", objective_sense::minimize,
                       [](scalar_view values) {
                           return 4.0 * values[0] * values[0] + 4.0 * values[1] * values[1];
                       })
        .add_objective("f2", objective_sense::minimize,
                       [](scalar_view values) {
                           const scalar shifted_x = values[0] - 5.0;
                           const scalar shifted_y = values[1] - 5.0;
                           return shifted_x * shifted_x + shifted_y * shifted_y;
                       })
        // (x-5)^2+y^2 <= 25  ->  25-(x-5)^2-y^2 >= 0.
        .add_restriction("inside_first_circle",
                         [](scalar_view values) {
                             const scalar shifted_x = values[0] - 5.0;
                             return 25.0 - shifted_x * shifted_x - values[1] * values[1];
                         })
        // (x-8)^2+(y+3)^2 >= 7.7  ->  (...) - 7.7 >= 0.
        .add_restriction("outside_second_circle", [](scalar_view values) {
            const scalar shifted_x = values[0] - 8.0;
            const scalar shifted_y = values[1] + 3.0;
            return shifted_x * shifted_x + shifted_y * shifted_y - 7.7;
        });

    benchmark_configuration configuration;
    configuration.initial_guess = {1.0, 1.0};
    return run_benchmark("WP27 Binh-Korn", problem_definition, configuration);
}
