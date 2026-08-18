#include "wp_example_support.hpp"

/**
 * @file WP42_Constr_Ex.cpp
 * @brief Two-variable constrained Constr-Ex multiobjective benchmark.
 *
 * The catalog objectives `x` and `(1 + y) / x` are minimized over
 * `0.1 <= x <= 1` and `0 <= y <= 5`. The published lower-bound inequalities
 * are translated into margins by subtracting their right-hand sides. The
 * configured initial design `(1,0)` is safely feasible for both restrictions.
 */

int main() {
    using namespace multobjopt;

    problem problem_definition;
    problem_definition.add_parameter("x", 0.1, 1.0)
        .add_parameter("y", 0.0, 5.0)
        .add_objective("f_1", objective_sense::minimize,
                       [](scalar_view values) { return values[0]; })
        .add_objective("f_2", objective_sense::minimize,
                       [](scalar_view values) { return (1.0 + values[1]) / values[0]; })
        .add_restriction("g_1",
                         [](scalar_view values) { return values[1] + 9.0 * values[0] - 6.0; })
        .add_restriction("g_2",
                         [](scalar_view values) { return -values[1] + 9.0 * values[0] - 1.0; });

    multobjopt_examples::benchmark_configuration configuration;
    configuration.initial_guess = {1.0, 0.0};
    return multobjopt_examples::run_benchmark("Constr-Ex", problem_definition, configuration);
}
