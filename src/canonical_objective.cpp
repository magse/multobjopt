#include "detail/optimizer_detail.hpp"

/**
 * @file canonical_objective.cpp
 * @brief Internal conversion of raw objectives to minimization direction.
 */

namespace multobjopt::detail {

/**
 * @brief Map a raw objective into the library's minimization convention.
 * @param objective Objective definition supplying the requested sense.
 * @param value Raw callback value retained in the public result.
 * @return @p value for minimization or its negation for maximization.
 *
 * Canonicalization is never written back to the raw objective vector. This
 * preserves the values expected by the overall validation callback and users.
 */
scalar canonical_objective(const objective_definition& objective, scalar value) noexcept {
    return objective.sense == objective_sense::minimize ? value : -value;
}

} // namespace multobjopt::detail
