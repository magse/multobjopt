#include <multobjopt/problem.hpp>

#include <utility>

/**
 * @file problem_add_parameter.cpp
 * @brief Parameter-building operations for multobjopt::problem.
 *
 * Both add_parameter() overloads live together so construction from a complete
 * definition and construction from individual values share one focused
 * implementation unit.
 */

namespace multobjopt {

/**
 * @brief Append one parameter definition to the problem.
 * @param parameter Definition to store in insertion order.
 * @return This problem, enabling fluent construction.
 *
 * Validation is deferred so applications can assemble problems incrementally
 * and receive all configuration checks at an explicit boundary.
 */
problem& problem::add_parameter(parameter_definition parameter) {
    parameters_.push_back(std::move(parameter));
    return *this;
}

/**
 * @brief Construct and append one parameter definition.
 * @param name Unique user-facing parameter name.
 * @param lower_bound Inclusive lower bound.
 * @param upper_bound Inclusive upper bound for continuous parameters.
 * @param resolution Grid spacing, or zero for a continuous parameter.
 * @return This problem, enabling fluent construction.
 */
problem& problem::add_parameter(std::string name, scalar lower_bound, scalar upper_bound,
                                scalar resolution) {
    return add_parameter({std::move(name), lower_bound, upper_bound, resolution});
}

} // namespace multobjopt
