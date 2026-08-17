#include "detail/optimizer_detail.hpp"

#include <limits>

/**
 * @file sortable_objective.cpp
 * @brief Internal objective canonicalization for deterministic sorting.
 */

namespace multobjopt::detail {

/**
 * @brief Produce a canonical value safe for deterministic sorting.
 * @param objective Objective definition supplying the requested sense.
 * @param value Raw objective value.
 * @return A finite canonical value, or positive infinity for invalid input.
 */
scalar sortable_objective(const objective_definition& objective, scalar value) noexcept {
    if (!finite(value)) {
        return std::numeric_limits<scalar>::infinity();
    }
    const scalar canonical = canonical_objective(objective, value);
    return finite(canonical) ? canonical : std::numeric_limits<scalar>::infinity();
}

} // namespace multobjopt::detail
