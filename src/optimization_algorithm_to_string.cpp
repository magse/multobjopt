#include "detail/optimizer_detail.hpp"

/**
 * @file optimization_algorithm_to_string.cpp
 * @brief Stable string formatting for optimization algorithm values.
 */

namespace multobjopt {

/**
 * @brief Convert an optimization algorithm to its stable API spelling.
 * @param algorithm Algorithm value to format.
 * @return Stable snake_case spelling, or `unknown` for an invalid value.
 */
std::string_view to_string(optimization_algorithm algorithm) noexcept {
    switch (algorithm) {
    case optimization_algorithm::automatic:
        return "automatic";
    case optimization_algorithm::simulated_annealing:
        return "simulated_annealing";
    case optimization_algorithm::genetic_algorithm:
        return "genetic_algorithm";
    case optimization_algorithm::box_method:
        return "box_method";
    case optimization_algorithm::gradient_descent:
        return "gradient_descent";
    }
    return "unknown";
}

} // namespace multobjopt
