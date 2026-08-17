#include "detail/optimizer_detail.hpp"

#include <stdexcept>

/**
 * @file evaluate_design.cpp
 * @brief Public validated complete-design evaluation operation.
 *
 * Objective callbacks run first, restriction callbacks run second, and the
 * optional overall validator receives both complete raw value vectors last.
 */

namespace multobjopt {

/**
 * @brief Validate and perform one complete design evaluation.
 * @param problem_definition Problem whose callbacks are invoked.
 * @param parameters Proposed design in parameter-definition order.
 * @param constraint_tolerance Allowed numerical restriction undershoot.
 * @return Normalized parameters, raw callback values, feasibility, violation,
 *         and scalar-compromise data.
 * @throws std::invalid_argument for invalid configuration or vector dimension.
 */
evaluated_design evaluate_design(const problem& problem_definition, scalar_view parameters,
                                 scalar constraint_tolerance) {
    problem_definition.validate();
    if (!detail::finite(constraint_tolerance) || constraint_tolerance < 0.0) {
        throw std::invalid_argument("constraint_tolerance must be finite and non-negative");
    }
    if (parameters.size() != problem_definition.parameters().size()) {
        throw std::invalid_argument("design vector size does not match the problem");
    }
    return detail::evaluate_unchecked(problem_definition, parameters, constraint_tolerance);
}

} // namespace multobjopt
