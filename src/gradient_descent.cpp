#include "detail/optimizer_detail.hpp"

#include "detail/gradient_descent_detail.hpp"

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

/**
 * @file gradient_descent.cpp
 * @brief Projected descent driven by bounded numerical gradients.
 *
 * This solver estimates derivatives in normalized parameter coordinates,
 * follows the negative gradient, and projects every trial back to the bounds
 * and any configured resolution lattice. Explicit restrictions and the overall
 * validation callback enter the finite-difference model through a quadratic
 * violation penalty; accepted steps still use the library's strict
 * feasibility-first ranking.
 *
 * Numerical probes and line-search trials all use detail::evaluator. The
 * implementation therefore counts complete designs consistently and refuses
 * to use a partial gradient when the evaluation budget is exhausted.
 */

namespace multobjopt::detail {
/**
 * @brief Optimize a problem with projected numerical-gradient descent.
 * @param problem_definition Validated optimization problem.
 * @param options Validated common optimizer options.
 * @return Best feasibility-first design, feasible Pareto archive, accounting
 *         totals, and the reason descent stopped.
 *
 * Each free coordinate is probed on both sides when bounds permit, falling
 * back to a one-sided difference near a boundary. Probe distance is at least
 * one resolution interval and no larger than the full parameter range. The
 * derivative denominator is expressed as a fraction of the coordinate range,
 * so gradient components are comparable across different engineering units.
 * Projection can collapse a probe onto the current grid point; such probes are
 * skipped without consuming a duplicate evaluation.
 *
 * The normalized negative-gradient direction is tested with a bounded
 * backtracking line search. A trial is accepted only when the shared
 * feasibility-first ordering improves, even though the gradient itself uses
 * the smooth penalized merit. Non-finite gradient norms cause numerical-failure
 * termination. A zero gradient, a step scale below its configured minimum, or
 * a projected direction unable to cross another grid boundary causes
 * convergence according to the existing step and stall rules.
 */
[[nodiscard]] optimization_result run_gradient_descent(const problem& problem_definition,
                                                       const optimizer_options& options) {
    evaluator problem_evaluator(problem_definition, options);
    auto current = problem_evaluator.evaluate(starting_point(problem_definition, options));
    const std::size_t free_count = free_parameter_count(problem_definition);
    if (free_count == 0) {
        return finish_result(optimization_algorithm::gradient_descent, problem_evaluator, 0,
                             termination_reason::converged);
    }
    if (!problem_evaluator.can_evaluate()) {
        return finish_result(optimization_algorithm::gradient_descent, problem_evaluator, 0,
                             termination_reason::evaluation_limit);
    }

    scalar step_scale = options.initial_step_scale;
    std::size_t iteration = 0;
    std::size_t stalled = 0;
    termination_reason reason = termination_reason::iteration_limit;
    for (; iteration < options.max_iterations && problem_evaluator.can_evaluate(); ++iteration) {
        const auto previous_best = problem_evaluator.best();
        std::vector<scalar> gradient(current.parameters.size(), 0.0);

        for (std::size_t index = 0;
             index < current.parameters.size() && problem_evaluator.can_evaluate(); ++index) {
            const auto& parameter = problem_definition.parameters()[index];
            const scalar range = parameter.upper_bound - parameter.lower_bound;
            if (range == 0.0) {
                continue;
            }
            const long double requested_delta =
                std::max(static_cast<long double>(options.finite_difference_step) *
                             static_cast<long double>(range),
                         static_cast<long double>(parameter.resolution));
            const scalar delta =
                static_cast<scalar>(std::min(requested_delta, static_cast<long double>(range)));
            auto plus_point = current.parameters;
            auto minus_point = current.parameters;
            plus_point[index] = static_cast<scalar>(std::clamp(
                static_cast<long double>(plus_point[index]) + static_cast<long double>(delta),
                static_cast<long double>(parameter.lower_bound),
                static_cast<long double>(parameter.upper_bound)));
            minus_point[index] = static_cast<scalar>(std::clamp(
                static_cast<long double>(minus_point[index]) - static_cast<long double>(delta),
                static_cast<long double>(parameter.lower_bound),
                static_cast<long double>(parameter.upper_bound)));
            plus_point = normalize_design_unchecked(problem_definition, plus_point);
            minus_point = normalize_design_unchecked(problem_definition, minus_point);

            const bool has_plus = plus_point[index] != current.parameters[index];
            const bool has_minus = minus_point[index] != current.parameters[index];
            evaluated_design plus;
            evaluated_design minus;
            if (has_plus && problem_evaluator.can_evaluate()) {
                plus = problem_evaluator.evaluate(plus_point);
            }
            if (has_minus && problem_evaluator.can_evaluate()) {
                minus = problem_evaluator.evaluate(minus_point);
            }

            if (has_plus && has_minus && !plus.parameters.empty() && !minus.parameters.empty()) {
                const scalar normalized_distance =
                    (plus.parameters[index] - minus.parameters[index]) / range;
                gradient[index] = (gradient_merit(plus, options.penalty_factor) -
                                   gradient_merit(minus, options.penalty_factor)) /
                                  normalized_distance;
            } else if (has_plus && !plus.parameters.empty()) {
                const scalar normalized_distance =
                    (plus.parameters[index] - current.parameters[index]) / range;
                gradient[index] = (gradient_merit(plus, options.penalty_factor) -
                                   gradient_merit(current, options.penalty_factor)) /
                                  normalized_distance;
            } else if (has_minus && !minus.parameters.empty()) {
                const scalar normalized_distance =
                    (current.parameters[index] - minus.parameters[index]) / range;
                gradient[index] = (gradient_merit(current, options.penalty_factor) -
                                   gradient_merit(minus, options.penalty_factor)) /
                                  normalized_distance;
            }
        }

        // A partial numerical gradient is not a valid convergence test.
        if (!problem_evaluator.can_evaluate()) {
            reason = termination_reason::evaluation_limit;
            ++iteration;
            break;
        }

        scalar gradient_norm_squared = 0.0;
        for (const auto value : gradient) {
            gradient_norm_squared += value * value;
        }
        const scalar gradient_norm = std::sqrt(gradient_norm_squared);
        if (!finite(gradient_norm)) {
            reason = termination_reason::numerical_failure;
            ++iteration;
            break;
        }
        if (gradient_norm <= options.gradient_tolerance) {
            reason = termination_reason::converged;
            ++iteration;
            break;
        }

        bool accepted = false;
        scalar trial_scale = step_scale;
        std::size_t line_search_attempts = 0;
        while (trial_scale >= options.minimum_step_scale && problem_evaluator.can_evaluate() &&
               line_search_attempts < 128) {
            ++line_search_attempts;
            auto trial_point = current.parameters;
            for (std::size_t index = 0; index < trial_point.size(); ++index) {
                const auto& parameter = problem_definition.parameters()[index];
                const scalar range = parameter.upper_bound - parameter.lower_bound;
                if (range == 0.0) {
                    continue;
                }
                const long double value = static_cast<long double>(trial_point[index]) -
                                          static_cast<long double>(trial_scale) *
                                              static_cast<long double>(range) *
                                              static_cast<long double>(gradient[index]) /
                                              static_cast<long double>(gradient_norm);
                trial_point[index] = static_cast<scalar>(
                    std::clamp(value, static_cast<long double>(parameter.lower_bound),
                               static_cast<long double>(parameter.upper_bound)));
            }
            trial_point = normalize_design_unchecked(problem_definition, trial_point);
            if (trial_point == current.parameters) {
                // A smaller step in the same direction cannot cross a grid
                // boundary that this step failed to cross.
                break;
            }
            auto trial = problem_evaluator.evaluate(trial_point);
            if (rank_before(trial, current)) {
                current = std::move(trial);
                step_scale =
                    std::min(options.initial_step_scale, trial_scale / options.line_search_decay);
                accepted = true;
                break;
            }
            trial_scale *= options.line_search_decay;
        }

        if (!accepted) {
            step_scale *= options.line_search_decay;
        }
        if (significant_improvement(problem_evaluator.best(), previous_best,
                                    options.improvement_tolerance)) {
            stalled = 0;
        } else {
            ++stalled;
        }

        if (!problem_evaluator.can_evaluate()) {
            reason = termination_reason::evaluation_limit;
            ++iteration;
            break;
        }
        if (step_scale < options.minimum_step_scale) {
            reason = termination_reason::converged;
            ++iteration;
            break;
        }
        if (stalled >= options.stall_iterations) {
            reason = termination_reason::stalled;
            ++iteration;
            break;
        }
    }

    return finish_result(optimization_algorithm::gradient_descent, problem_evaluator, iteration,
                         reason);
}

} // namespace multobjopt::detail
