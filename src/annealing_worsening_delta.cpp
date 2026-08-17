#include "detail/simulated_annealing_detail.hpp"

#include <algorithm>

/**
 * @file annealing_worsening_delta.cpp
 * @brief Feasibility-aware acceptance energy for simulated annealing.
 */

namespace multobjopt::detail {

/**
 * @brief Compute the non-negative annealing energy needed for a worse move.
 * @param candidate Proposed, fully evaluated design.
 * @param current Design at the current Markov-chain state.
 * @param penalty_factor Scaling applied to constraint violations.
 * @return Zero for a feasibility improvement, otherwise the scalar objective
 *         or violation increase used by the Metropolis acceptance rule.
 *
 * Crossing from feasibility into infeasibility has an additional unit cost,
 * so even an infinitesimal violation remains distinguishable from a feasible
 * point. The caller rejects non-finite values instead of passing them through
 * the exponential acceptance expression.
 */
scalar annealing_worsening_delta(const evaluated_design& candidate, const evaluated_design& current,
                                 scalar penalty_factor) noexcept {
    if (candidate.feasible && current.feasible) {
        return std::max(0.0, candidate.scalarized_objective - current.scalarized_objective);
    }
    if (!candidate.feasible && !current.feasible) {
        return std::max(0.0, candidate.total_violation - current.total_violation) * penalty_factor;
    }
    if (!candidate.feasible && current.feasible) {
        return penalty_factor * (1.0 + candidate.total_violation);
    }
    return 0.0;
}

} // namespace multobjopt::detail
