#include "detail/optimizer_detail.hpp"

/**
 * @file termination_reason_to_string.cpp
 * @brief Stable string formatting for optimization termination reasons.
 */

namespace multobjopt {

/**
 * @brief Convert a termination reason to its stable API spelling.
 * @param reason Termination reason to format.
 * @return Stable snake_case spelling, or `unknown` for an invalid value.
 */
std::string_view to_string(termination_reason reason) noexcept {
    switch (reason) {
    case termination_reason::evaluation_limit:
        return "evaluation_limit";
    case termination_reason::iteration_limit:
        return "iteration_limit";
    case termination_reason::stalled:
        return "stalled";
    case termination_reason::converged:
        return "converged";
    case termination_reason::no_feasible_design:
        return "no_feasible_design";
    case termination_reason::numerical_failure:
        return "numerical_failure";
    }
    return "unknown";
}

} // namespace multobjopt
