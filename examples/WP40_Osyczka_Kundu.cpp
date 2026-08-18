#include "wp_example_support.hpp"

/**
 * @file WP40_Osyczka_Kundu.cpp
 * @brief Six-variable constrained Osyczka-Kundu benchmark.
 *
 * The two catalog objectives are minimized over their heterogeneous parameter
 * bounds. Each of the six published inequalities is represented as its left-
 * hand feasible margin, so a nonnegative callback value means the original
 * inequality is satisfied. The configured initial design `(2,2,3,0,1,0)` is
 * inside the bounds and satisfies all six restrictions.
 */

int main() {
    using namespace multobjopt;

    problem problem_definition;
    problem_definition.add_parameter("x_1", 0.0, 10.0)
        .add_parameter("x_2", 0.0, 10.0)
        .add_parameter("x_3", 1.0, 5.0)
        .add_parameter("x_4", 0.0, 6.0)
        .add_parameter("x_5", 1.0, 5.0)
        .add_parameter("x_6", 0.0, 10.0)
        .add_objective("f_1", objective_sense::minimize,
                       [](scalar_view values) {
                           const scalar first = values[0] - 2.0;
                           const scalar second = values[1] - 2.0;
                           const scalar third = values[2] - 1.0;
                           const scalar fourth = values[3] - 4.0;
                           const scalar fifth = values[4] - 1.0;
                           return -25.0 * first * first - second * second - third * third -
                                  fourth * fourth - fifth * fifth;
                       })
        .add_objective("f_2", objective_sense::minimize,
                       [](scalar_view values) {
                           scalar sum_of_squares = 0.0;
                           for (const scalar value : values) {
                               sum_of_squares += value * value;
                           }
                           return sum_of_squares;
                       })
        .add_restriction("g_1", [](scalar_view values) { return values[0] + values[1] - 2.0; })
        .add_restriction("g_2", [](scalar_view values) { return 6.0 - values[0] - values[1]; })
        .add_restriction("g_3", [](scalar_view values) { return 2.0 - values[1] + values[0]; })
        .add_restriction("g_4",
                         [](scalar_view values) { return 2.0 - values[0] + 3.0 * values[1]; })
        .add_restriction("g_5",
                         [](scalar_view values) {
                             const scalar difference = values[2] - 3.0;
                             return 4.0 - difference * difference - values[3];
                         })
        .add_restriction("g_6", [](scalar_view values) {
            const scalar difference = values[4] - 3.0;
            return difference * difference + values[5] - 4.0;
        });

    multobjopt_examples::benchmark_configuration configuration;
    configuration.initial_guess = {2.0, 2.0, 3.0, 0.0, 1.0, 0.0};
    return multobjopt_examples::run_benchmark("Osyczka-Kundu", problem_definition, configuration);
}
