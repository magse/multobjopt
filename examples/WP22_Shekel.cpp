/**
 * @file WP22_Shekel.cpp
 * @brief Four-dimensional, ten-center Shekel benchmark from the Wikipedia form.
 *
 * Wikipedia specifies the positive reciprocal-sum formula but leaves the
 * coefficient arrays and center count unspecified. This example supplies the
 * common four-dimensional `m = 10` coefficient set and its conventional finite
 * `[0, 10]^4` search box. The positive sum is therefore MAXIMIZED; this is
 * equivalent to the widespread benchmark convention that minimizes its
 * negation.
 */

#include "wp_example_support.hpp"

#include <array>
#include <string>

int main() {
    using namespace multobjopt;
    using namespace multobjopt_examples;

    problem problem_definition;
    for (std::size_t index = 0; index < 4; ++index) {
        problem_definition.add_parameter("x" + std::to_string(index + 1), 0.0, 10.0);
    }
    problem_definition.add_objective(
        "shekel_positive_sum", objective_sense::maximize, [](scalar_view values) {
            constexpr std::array<std::array<scalar, 4>, 10> centers{{
                {{4.0, 4.0, 4.0, 4.0}},
                {{1.0, 1.0, 1.0, 1.0}},
                {{8.0, 8.0, 8.0, 8.0}},
                {{6.0, 6.0, 6.0, 6.0}},
                {{3.0, 7.0, 3.0, 7.0}},
                {{2.0, 9.0, 2.0, 9.0}},
                {{5.0, 5.0, 3.0, 3.0}},
                {{8.0, 1.0, 8.0, 1.0}},
                {{6.0, 2.0, 6.0, 2.0}},
                {{7.0, 3.6, 7.0, 3.6}},
            }};
            constexpr std::array<scalar, 10> offsets{
                0.1, 0.2, 0.2, 0.4, 0.4, 0.6, 0.3, 0.7, 0.5, 0.5,
            };

            scalar sum = 0.0;
            for (std::size_t center = 0; center < centers.size(); ++center) {
                scalar denominator = offsets[center];
                for (std::size_t coordinate = 0; coordinate < centers[center].size();
                     ++coordinate) {
                    const scalar difference = values[coordinate] - centers[center][coordinate];
                    denominator += difference * difference;
                }
                sum += 1.0 / denominator;
            }
            return sum;
        });

    return run_benchmark("WP22 Shekel (4D, m=10, positive sum)", problem_definition);
}
