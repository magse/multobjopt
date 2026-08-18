#include "wp_example_support.hpp"

/**
 * @file WP33_Schaffer_N2_Multiobjective.cpp
 * @brief Piecewise one-variable Schaffer N2 multiobjective benchmark.
 *
 * The first minimized objective follows all four catalog branches with their
 * stated closed and open interval boundaries. The second is the squared
 * distance from five, and the search interval is `[-5, 10]`.
 */

int main() {
    using namespace multobjopt;

    problem problem_definition;
    problem_definition.add_parameter("x", -5.0, 10.0)
        .add_objective("f_1", objective_sense::minimize,
                       [](scalar_view values) {
                           const scalar x = values[0];
                           if (x <= 1.0) {
                               return -x;
                           }
                           if (x <= 3.0) {
                               return x - 2.0;
                           }
                           if (x <= 4.0) {
                               return 4.0 - x;
                           }
                           return x - 4.0;
                       })
        .add_objective("f_2", objective_sense::minimize, [](scalar_view values) {
            const scalar difference = values[0] - 5.0;
            return difference * difference;
        });

    return multobjopt_examples::run_benchmark("Schaffer N2 multiobjective", problem_definition);
}
