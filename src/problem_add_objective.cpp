#include <multobjopt/problem.hpp>

#include <utility>

/**
 * @file problem_add_objective.cpp
 * @brief Objective-building operations for multobjopt::problem.
 *
 * Both add_objective() overloads live together so the convenience overload
 * delegates to the definition-based operation without duplicating storage
 * behavior.
 */

namespace multobjopt {

/**
 * @brief Append one objective definition to the problem.
 * @param objective Definition to store in insertion order.
 * @return This problem, enabling fluent construction.
 */
problem& problem::add_objective(objective_definition objective) {
    objectives_.push_back(std::move(objective));
    return *this;
}

/**
 * @brief Construct and append one scalar objective.
 * @param name Unique user-facing objective name.
 * @param sense Direction in which the objective is optimized.
 * @param function User callback that computes the raw objective value.
 * @param weight Positive scalar-compromise weight.
 * @return This problem, enabling fluent construction.
 */
problem& problem::add_objective(std::string name, objective_sense sense, scalar_function function,
                                scalar weight) {
    return add_objective({std::move(name), sense, std::move(function), weight});
}

} // namespace multobjopt
