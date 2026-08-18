#include "detail/optimizer_detail.hpp"

/**
 * @file finish_result.cpp
 * @brief Common public optimization-result assembly.
 */

namespace multobjopt::detail {

/**
 * @brief Assemble the public result shared by every optimizer.
 * @param algorithm Algorithm that actually ran.
 * @param problem_evaluator Completed evaluator state.
 * @param iterations Number of outer iterations completed.
 * @param reason Method-specific termination reason.
 * @return Complete optimization result with a deterministically ordered archive.
 *
 * Absence of a feasible archived design overrides the method-specific reason
 * with `no_feasible_design`, while preserving its best infeasible design and
 * exact evaluation and iteration counts.
 */
optimization_result finish_result(optimization_algorithm algorithm,
                                  const evaluator& problem_evaluator, std::size_t iterations,
                                  termination_reason reason) {
    optimization_result result;
    result.algorithm_used = algorithm;
    result.best_design = problem_evaluator.best();
    result.pareto_front = problem_evaluator.archive();
    result.evaluations = problem_evaluator.evaluations();
    result.iterations = iterations;
    result.reason = result.pareto_front.empty() ? termination_reason::no_feasible_design : reason;
    result.evaluation_history = problem_evaluator.evaluation_history();
    return result;
}

} // namespace multobjopt::detail
