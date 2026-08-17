#include <multobjopt/problem.hpp>

/**
 * @file problem_parameters.cpp
 * @brief Parameter-definition access for multobjopt::problem.
 */

namespace multobjopt {

/**
 * @brief Return parameter definitions in their callback-vector order.
 * @return Const reference to the definitions owned by this problem.
 */
const std::vector<parameter_definition>& problem::parameters() const noexcept {
    return parameters_;
}

} // namespace multobjopt
