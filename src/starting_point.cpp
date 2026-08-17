#include "detail/optimizer_detail.hpp"

/**
 * @file starting_point.cpp
 * @brief Common initial-design selection for all optimizers.
 */

namespace multobjopt::detail {

/**
 * @brief Resolve the common initial design for an optimizer.
 * @param problem_definition Validated problem.
 * @param options Validated options with an optional initial guess.
 * @return The normalized initial guess, or normalized interval midpoint.
 */
std::vector<scalar> starting_point(const problem& problem_definition,
                                   const optimizer_options& options) {
    if (!options.initial_guess.empty()) {
        return normalize_design_unchecked(problem_definition, options.initial_guess);
    }
    return midpoint(problem_definition);
}

} // namespace multobjopt::detail
