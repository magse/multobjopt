#include <multobjopt/problem.hpp>

/**
 * @file problem_objectives.cpp
 * @brief Objective-definition access for multobjopt::problem.
 */

namespace multobjopt {

/**
 * @brief Return objective definitions in their callback-vector order.
 * @return Const reference to the definitions owned by this problem.
 */
const std::vector<objective_definition>& problem::objectives() const noexcept {
    return objectives_;
}

} // namespace multobjopt
