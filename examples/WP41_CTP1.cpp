#include "wp_example_support.hpp"

#include <cmath>

/**
 * @file WP41_CTP1.cpp
 * @brief Two-variable constrained CTP1 multiobjective benchmark.
 *
 * Both variables retain the catalog domain `[0, 1]`, and both objectives are
 * minimized. Wikipedia states each nonlinear constraint as a ratio greater
 * than or equal to one; subtracting one produces the equivalent multobjopt
 * restriction margin. The initial design `(0,0)` makes both margins positive.
 */

namespace {

/**
 * @brief Evaluate CTP1's second catalog objective.
 * @param values Two design variables ordered as x and y.
 * @return `(1 + y) * exp(-x / (1 + y))`.
 */
multobjopt::scalar ctp1_second_objective(multobjopt::scalar_view values) {
    return (1.0 + values[1]) * std::exp(-values[0] / (1.0 + values[1]));
}

} // namespace

int main() {
    using namespace multobjopt;

    problem problem_definition;
    problem_definition.add_parameter("x", 0.0, 1.0)
        .add_parameter("y", 0.0, 1.0)
        .add_objective("f_1", objective_sense::minimize,
                       [](scalar_view values) { return values[0]; })
        .add_objective("f_2", objective_sense::minimize,
                       [](scalar_view values) { return ctp1_second_objective(values); })
        .add_restriction("g_1",
                         [](scalar_view values) {
                             const scalar denominator = 0.858 * std::exp(-0.541 * values[0]);
                             return ctp1_second_objective(values) / denominator - 1.0;
                         })
        .add_restriction("g_2", [](scalar_view values) {
            const scalar denominator = 0.728 * std::exp(-0.295 * values[0]);
            return ctp1_second_objective(values) / denominator - 1.0;
        });

    multobjopt_examples::benchmark_configuration configuration;
    configuration.initial_guess = {0.0, 0.0};
    return multobjopt_examples::run_benchmark("CTP1", problem_definition, configuration);
}
