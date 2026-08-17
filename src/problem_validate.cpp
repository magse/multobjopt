#include "detail/optimizer_detail.hpp"

#include <algorithm>

/**
 * @file problem_validate.cpp
 * @brief Structural and numerical validation for multobjopt::problem.
 *
 * Problem validation deliberately checks definitions in insertion order. That
 * ordering makes diagnostics deterministic and matches the parameter,
 * objective, and restriction vectors supplied to user callbacks.
 */

namespace multobjopt {

/**
 * @brief Validate every structural and numerical problem invariant.
 * @throws std::invalid_argument if a required definition is missing, a name is
 *         empty or duplicated within its category, a callback is empty, or a
 *         numerical definition cannot represent the documented design space.
 *
 * The parameter-grid checks are intentionally performed using the public
 * scalar type. They reject grids whose quotient overflows before optimizers
 * attempt to calculate or sample an integer lattice index.
 */
void problem::validate() const {
    detail::require(!parameters_.empty(), "a problem requires at least one parameter");
    detail::require(parameters_.size() <= 25, "a problem supports at most 25 parameters");
    detail::require(!objectives_.empty(), "a problem requires at least one objective");

    std::vector<std::string> names;
    names.reserve(parameters_.size());
    for (const auto& parameter : parameters_) {
        detail::require(!parameter.name.empty(), "parameter names must not be empty");
        detail::require(detail::finite(parameter.lower_bound) &&
                            detail::finite(parameter.upper_bound),
                        "parameter bounds must be finite");
        detail::require(parameter.lower_bound <= parameter.upper_bound,
                        "a parameter lower bound must not exceed its upper bound");
        detail::require(detail::finite(parameter.upper_bound - parameter.lower_bound),
                        "a parameter range must be finite");
        detail::require(detail::finite(parameter.resolution) && parameter.resolution >= 0.0,
                        "parameter resolution must be finite and non-negative");
        if (parameter.resolution > 0.0) {
            detail::require(parameter.lower_bound < parameter.upper_bound,
                            "a fixed parameter cannot have a positive resolution");
            detail::require(parameter.resolution <= parameter.upper_bound - parameter.lower_bound,
                            "parameter resolution must not exceed its range");
            detail::require(detail::finite((parameter.upper_bound - parameter.lower_bound) /
                                           parameter.resolution),
                            "parameter resolution creates an unrepresentable grid");
        }
        detail::require(std::find(names.begin(), names.end(), parameter.name) == names.end(),
                        "parameter names must be unique");
        names.push_back(parameter.name);
    }

    names.clear();
    for (const auto& objective : objectives_) {
        detail::require(!objective.name.empty(), "objective names must not be empty");
        detail::require(static_cast<bool>(objective.function),
                        "objective functions must not be empty");
        detail::require(detail::finite(objective.weight) && objective.weight > 0.0,
                        "objective weights must be finite and positive");
        detail::require(std::find(names.begin(), names.end(), objective.name) == names.end(),
                        "objective names must be unique");
        names.push_back(objective.name);
    }

    names.clear();
    for (const auto& restriction : restrictions_) {
        detail::require(!restriction.name.empty(), "restriction names must not be empty");
        detail::require(static_cast<bool>(restriction.function),
                        "restriction functions must not be empty");
        detail::require(detail::finite(restriction.scale) && restriction.scale > 0.0,
                        "restriction scales must be finite and positive");
        detail::require(std::find(names.begin(), names.end(), restriction.name) == names.end(),
                        "restriction names must be unique");
        names.push_back(restriction.name);
    }
}

} // namespace multobjopt
