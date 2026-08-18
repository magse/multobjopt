#include "wp_example_support.hpp"

/**
 * @file WP30_Test_Function_4.cpp
 * @brief Constrained two-objective benchmark named Test Function 4.
 *
 * Both catalog objectives are minimized over `[-7, 4]^2`. The three linear
 * inequalities are written directly as restriction margins whose nonnegative
 * values denote feasible designs, matching both the source notation and the
 * multobjopt restriction convention.
 */

int main() {
    using namespace multobjopt;

    problem problem_definition;
    problem_definition.add_parameter("x", -7.0, 4.0)
        .add_parameter("y", -7.0, 4.0)
        .add_objective("f_1", objective_sense::minimize,
                       [](scalar_view values) { return values[0] * values[0] - values[1]; })
        .add_objective("f_2", objective_sense::minimize,
                       [](scalar_view values) { return -0.5 * values[0] - values[1] - 1.0; })
        .add_restriction("g_1",
                         [](scalar_view values) { return 6.5 - values[0] / 6.0 - values[1]; })
        .add_restriction("g_2",
                         [](scalar_view values) { return 7.5 - 0.5 * values[0] - values[1]; })
        .add_restriction("g_3",
                         [](scalar_view values) { return 30.0 - 5.0 * values[0] - values[1]; });

    multobjopt_examples::benchmark_configuration configuration;
    configuration.initial_guess = {0.0, 0.0};
    return multobjopt_examples::run_benchmark("Test Function 4", problem_definition, configuration);
}
