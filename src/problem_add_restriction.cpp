#include <multobjopt/problem.hpp>

#include <utility>

/**
 * @file problem_add_restriction.cpp
 * @brief Restriction-building operations for multobjopt::problem.
 *
 * The definition and convenience overloads are kept together because they
 * implement the same public operation and preserve one insertion-order path.
 */

namespace multobjopt {

/**
 * @brief Append one restriction definition to the problem.
 * @param restriction Definition to store in insertion order.
 * @return This problem, enabling fluent construction.
 */
problem& problem::add_restriction(restriction_definition restriction) {
    restrictions_.push_back(std::move(restriction));
    return *this;
}

/**
 * @brief Construct and append one scalar restriction.
 * @param name Unique user-facing restriction name.
 * @param function User callback whose non-negative result is feasible.
 * @param scale Positive normalization scale for violation magnitude.
 * @return This problem, enabling fluent construction.
 */
problem& problem::add_restriction(std::string name, scalar_function function, scalar scale) {
    return add_restriction({std::move(name), std::move(function), scale});
}

} // namespace multobjopt
