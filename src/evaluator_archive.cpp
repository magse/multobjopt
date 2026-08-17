#include "detail/optimizer_detail.hpp"

#include <algorithm>

/**
 * @file evaluator_archive.cpp
 * @brief Deterministic presentation of the shared Pareto archive.
 */

namespace multobjopt::detail {

/**
 * @brief Copy the Pareto archive into deterministic presentation order.
 * @return Feasible nondominated designs ordered by canonical objectives, with
 *         parameter vectors as the final tie-breaker.
 *
 * Internal insertion order depends on search traversal. Sorting only the copy
 * preserves efficient updates while making public results reproducible.
 */
std::vector<evaluated_design> evaluator::archive() const {
    auto result = archive_;
    std::sort(result.begin(), result.end(), [&](const auto& lhs, const auto& rhs) {
        for (std::size_t index = 0; index < lhs.objectives.size(); ++index) {
            const scalar lhs_value =
                canonical_objective(problem_.objectives()[index], lhs.objectives[index]);
            const scalar rhs_value =
                canonical_objective(problem_.objectives()[index], rhs.objectives[index]);
            if (lhs_value != rhs_value) {
                return lhs_value < rhs_value;
            }
        }
        return lexicographically_less(lhs.parameters, rhs.parameters);
    });
    return result;
}

} // namespace multobjopt::detail
