#include "detail/optimizer_detail.hpp"

#include <stdexcept>

/**
 * @file dominates.cpp
 * @brief Feasibility-aware Pareto dominance comparison.
 */

namespace multobjopt {

/**
 * @brief Test feasibility-aware Pareto dominance under the problem's senses.
 * @param problem_definition Problem defining the objective directions.
 * @param lhs Potentially dominating design.
 * @param rhs Potentially dominated design.
 * @param tolerance Minimum objective or violation improvement considered strict.
 * @return True exactly when @p lhs dominates @p rhs.
 * @throws std::invalid_argument for an invalid tolerance or mismatched feasible
 *         objective-vector dimensions.
 *
 * Component-wise non-worseness is exact, while @p tolerance affects only the
 * required strict improvement. Keeping these roles separate preserves the
 * transitivity needed by nondominated sorting and Pareto archive maintenance.
 */
bool dominates(const problem& problem_definition, const evaluated_design& lhs,
               const evaluated_design& rhs, scalar tolerance) {
    if (!detail::finite(tolerance) || tolerance < 0.0) {
        throw std::invalid_argument("dominance tolerance must be finite and non-negative");
    }
    if (lhs.feasible != rhs.feasible) {
        return lhs.feasible;
    }
    if (!lhs.feasible) {
        return lhs.total_violation + tolerance < rhs.total_violation;
    }
    if (lhs.objectives.size() != problem_definition.objectives().size() ||
        rhs.objectives.size() != problem_definition.objectives().size()) {
        throw std::invalid_argument("objective vector size does not match the problem");
    }

    bool strictly_better = false;
    for (std::size_t index = 0; index < lhs.objectives.size(); ++index) {
        const scalar lhs_value = detail::canonical_objective(problem_definition.objectives()[index],
                                                             lhs.objectives[index]);
        const scalar rhs_value = detail::canonical_objective(problem_definition.objectives()[index],
                                                             rhs.objectives[index]);
        if (!detail::finite(lhs_value) || !detail::finite(rhs_value)) {
            return false;
        }
        // Exact component-wise non-worseness keeps dominance transitive. The
        // tolerance applies only to deciding whether an improvement is strict.
        if (lhs_value > rhs_value) {
            return false;
        }
        if (lhs_value + tolerance < rhs_value) {
            strictly_better = true;
        }
    }
    return strictly_better;
}

} // namespace multobjopt
