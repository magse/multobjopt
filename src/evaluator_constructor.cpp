#include "detail/optimizer_detail.hpp"

/**
 * @file evaluator_constructor.cpp
 * @brief Construction of the shared budgeted optimizer evaluator.
 */

namespace multobjopt::detail {

/**
 * @brief Bind a budgeted evaluator to a validated problem and options object.
 * @param problem_definition Problem that outlives this evaluator.
 * @param options Options that outlive this evaluator.
 *
 * Construction itself performs no callbacks. The first successful evaluate()
 * call establishes the best design and may establish the Pareto archive.
 */
evaluator::evaluator(const problem& problem_definition, const optimizer_options& options)
    : problem_(problem_definition), options_(options) {}

} // namespace multobjopt::detail
