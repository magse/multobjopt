#include <multobjopt/problem.hpp>

/**
 * @file problem_restrictions.cpp
 * @brief Restriction-definition access for multobjopt::problem.
 */

namespace multobjopt {

/**
 * @brief Return restriction definitions in their callback-vector order.
 * @return Const reference to the definitions owned by this problem.
 */
const std::vector<restriction_definition>& problem::restrictions() const noexcept {
    return restrictions_;
}

} // namespace multobjopt
