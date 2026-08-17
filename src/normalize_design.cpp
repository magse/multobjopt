#include "detail/optimizer_detail.hpp"

#include <vector>

/**
 * @file normalize_design.cpp
 * @brief Public validated design-projection operation.
 *
 * Parameter grids are anchored at the lower bound. A non-aligned upper bound
 * remains a bound and is not introduced as an additional grid point.
 */

namespace multobjopt {

/**
 * @brief Validate and normalize a user-supplied design vector.
 * @param problem_definition Problem supplying component bounds and resolutions.
 * @param parameters Proposed design in definition order.
 * @return A bounded and grid-aligned design.
 * @throws std::invalid_argument for an invalid problem, vector dimension, or
 *         nonfinite parameter value.
 */
std::vector<scalar> normalize_design(const problem& problem_definition, scalar_view parameters) {
    problem_definition.validate();
    return detail::normalize_design_unchecked(problem_definition, parameters);
}

} // namespace multobjopt
