#include "wp_example_support.hpp"

/**
 * @file WP32_Schaffer_N1_Multiobjective.cpp
 * @brief One-variable Schaffer N1 multiobjective benchmark.
 *
 * The benchmark minimizes the squared distance from zero and the squared
 * distance from two. Wikipedia describes a symmetric `[-A, A]` domain and
 * notes successful values from 10 to 100000; this example deliberately uses
 * the smallest catalog value, `A = 10`, for a compact demonstration.
 */

int main() {
    using namespace multobjopt;

    constexpr scalar search_limit = 10.0;

    problem problem_definition;
    problem_definition.add_parameter("x", -search_limit, search_limit)
        .add_objective("f_1", objective_sense::minimize,
                       [](scalar_view values) { return values[0] * values[0]; })
        .add_objective("f_2", objective_sense::minimize, [](scalar_view values) {
            const scalar difference = values[0] - 2.0;
            return difference * difference;
        });

    return multobjopt_examples::run_benchmark("Schaffer N1 multiobjective", problem_definition);
}
