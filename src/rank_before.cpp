#include "detail/optimizer_detail.hpp"

/**
 * @file rank_before.cpp
 * @brief Internal feasibility-first total ordering of evaluated designs.
 */

namespace multobjopt::detail {

/**
 * @brief Apply the common feasibility-first total ordering between designs.
 * @param lhs Candidate left design.
 * @param rhs Candidate right design.
 * @return True when @p lhs must sort before @p rhs.
 *
 * Feasibility wins first. Infeasible designs compare total violation, all ties
 * then compare scalar compromise, and parameter vectors provide stable final
 * ordering. This function underpins reproducibility throughout the algorithms.
 */
bool rank_before(const evaluated_design& lhs, const evaluated_design& rhs) noexcept {
    if (lhs.feasible != rhs.feasible) {
        return lhs.feasible;
    }
    if (!lhs.feasible && lhs.total_violation != rhs.total_violation) {
        return lhs.total_violation < rhs.total_violation;
    }
    if (lhs.scalarized_objective != rhs.scalarized_objective) {
        return lhs.scalarized_objective < rhs.scalarized_objective;
    }
    return lexicographically_less(lhs.parameters, rhs.parameters);
}

} // namespace multobjopt::detail
