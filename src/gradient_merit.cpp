#include "detail/gradient_descent_detail.hpp"

#include <limits>

/**
 * @file gradient_merit.cpp
 * @brief Penalized scalar merit calculation for gradient descent.
 */

namespace multobjopt::detail {

/**
 * @brief Convert an evaluated design to the penalized descent merit.
 * @param design Evaluated design whose scalar merit is requested.
 * @param penalty_factor Positive multiplier for squared total violation.
 * @return Scalarized objective plus quadratic violation penalty, or positive
 *         infinity if any input or arithmetic result is non-finite.
 *
 * Returning infinity rather than propagating NaN keeps finite-difference and
 * line-search comparisons deterministic. Squaring the violation increasingly
 * discourages movement away from feasibility while leaving feasible objective
 * values unchanged.
 */
scalar gradient_merit(const evaluated_design& design, scalar penalty_factor) noexcept {
    if (!finite(design.scalarized_objective) || !finite(design.total_violation)) {
        return std::numeric_limits<scalar>::infinity();
    }
    const scalar penalty = penalty_factor * design.total_violation * design.total_violation;
    if (!finite(penalty)) {
        return std::numeric_limits<scalar>::infinity();
    }
    return design.scalarized_objective + penalty;
}

} // namespace multobjopt::detail
