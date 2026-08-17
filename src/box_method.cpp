#include "detail/box_method_detail.hpp"
#include "detail/optimizer_detail.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <utility>
#include <vector>

/**
 * @file box_method.cpp
 * @brief Feasible-complex Box search for bounded constrained problems.
 *
 * The Box method maintains a collection of feasible evaluated designs. At
 * each iteration it reflects the worst member through the centroid of all
 * other members, repairs infeasible reflections toward known feasible points,
 * and contracts unsuccessful trials. This makes the solver suitable for
 * nonlinear restrictions and Boolean validation callbacks for which numerical
 * gradients are unavailable.
 *
 * Every initial sample, repair point, reflection, and contraction consumes one
 * complete evaluation through detail::evaluator. Each repair loop checks the
 * remaining budget before evaluating another point.
 */

namespace multobjopt::detail {
/**
 * @brief Optimize a constrained problem with the feasible-complex Box method.
 * @param problem_definition Validated optimization problem.
 * @param options Validated common optimizer options.
 * @return Best feasibility-first design, feasible Pareto archive, accounting
 *         totals, and the reason the complex search stopped.
 *
 * Initialization first searches globally for one feasible seed. Further random
 * members that violate restrictions are repeatedly bisected toward that seed.
 * The main loop orders the complex by the shared feasibility-first scalar rank,
 * computes a centroid excluding the worst member, and tests a bounded reflected
 * point. An infeasible reflection is repaired toward the centroid and then,
 * for non-convex validation regions, toward the best known feasible member. An
 * unsuccessful reflection is contracted and repaired in the same manner.
 *
 * The centroid uses an incremental mean to avoid overflowing when bounds are
 * large and have the same sign. Convergence is based on normalized complex
 * diameter; scalar improvement separately drives the stall counter. If no
 * feasible seed can be found within the budget, shared result construction
 * reports that no feasible design was obtained.
 */
[[nodiscard]] optimization_result run_box_method(const problem& problem_definition,
                                                 const optimizer_options& options) {
    evaluator problem_evaluator(problem_definition, options);
    std::mt19937_64 generator(options.random_seed);
    const std::size_t free_count = free_parameter_count(problem_definition);
    const std::size_t requested_size = options.complex_size == 0
                                           ? std::max<std::size_t>(4, 2 * free_count + 1)
                                           : options.complex_size;

    auto feasible_seed = problem_evaluator.evaluate(starting_point(problem_definition, options));
    if (free_count == 0) {
        return finish_result(optimization_algorithm::box_method, problem_evaluator, 0,
                             termination_reason::converged);
    }

    // The Box complex is maintained as a feasible set. If the supplied start
    // is invalid, global samples first search for a feasible seed.
    while (!feasible_seed.feasible && problem_evaluator.can_evaluate()) {
        auto sample = problem_evaluator.evaluate(random_point(problem_definition, generator));
        if (rank_before(sample, feasible_seed)) {
            feasible_seed = std::move(sample);
        }
    }
    if (!feasible_seed.feasible) {
        return finish_result(optimization_algorithm::box_method, problem_evaluator, 0,
                             termination_reason::evaluation_limit);
    }

    std::vector<evaluated_design> complex;
    complex.reserve(requested_size);
    complex.push_back(feasible_seed);
    while (complex.size() < requested_size && problem_evaluator.can_evaluate()) {
        auto candidate = problem_evaluator.evaluate(random_point(problem_definition, generator));
        for (std::size_t repair = 0;
             !candidate.feasible && repair < 24 && problem_evaluator.can_evaluate(); ++repair) {
            auto repaired = candidate.parameters;
            for (std::size_t parameter = 0; parameter < repaired.size(); ++parameter) {
                repaired[parameter] +=
                    (feasible_seed.parameters[parameter] - repaired[parameter]) * 0.5;
            }
            repaired = normalize_design_unchecked(problem_definition, repaired);
            if (repaired == candidate.parameters) {
                break;
            }
            candidate = problem_evaluator.evaluate(repaired);
        }
        if (candidate.feasible) {
            complex.push_back(std::move(candidate));
        }
    }

    if (complex.size() == 1) {
        return finish_result(optimization_algorithm::box_method, problem_evaluator, 0,
                             problem_evaluator.can_evaluate()
                                 ? termination_reason::converged
                                 : termination_reason::evaluation_limit);
    }
    if (!problem_evaluator.can_evaluate()) {
        return finish_result(optimization_algorithm::box_method, problem_evaluator, 0,
                             termination_reason::evaluation_limit);
    }

    std::size_t iteration = 0;
    std::size_t stalled = 0;
    termination_reason reason = termination_reason::iteration_limit;
    const scalar diameter_tolerance = std::max(1.0e-8, std::sqrt(options.improvement_tolerance));
    for (; iteration < options.max_iterations && problem_evaluator.can_evaluate(); ++iteration) {
        const auto previous_best = problem_evaluator.best();
        std::sort(complex.begin(), complex.end(), rank_before);
        const auto& worst = complex.back();
        std::vector<scalar> centroid = complex.front().parameters;
        for (std::size_t member = 0; member + 1 < complex.size(); ++member) {
            if (member == 0) {
                continue;
            }
            for (std::size_t parameter = 0; parameter < centroid.size(); ++parameter) {
                // Incremental means avoid overflow for large, same-sign bounds.
                centroid[parameter] +=
                    (complex[member].parameters[parameter] - centroid[parameter]) /
                    static_cast<scalar>(member + 1);
            }
        }

        std::vector<scalar> reflected(centroid.size());
        for (std::size_t parameter = 0; parameter < reflected.size(); ++parameter) {
            const auto& definition = problem_definition.parameters()[parameter];
            const long double value = static_cast<long double>(centroid[parameter]) +
                                      static_cast<long double>(options.reflection_factor) *
                                          (static_cast<long double>(centroid[parameter]) -
                                           static_cast<long double>(worst.parameters[parameter]));
            reflected[parameter] = static_cast<scalar>(
                std::clamp(value, static_cast<long double>(definition.lower_bound),
                           static_cast<long double>(definition.upper_bound)));
        }
        auto candidate = problem_evaluator.evaluate(reflected);

        // Move an infeasible reflection toward the feasible centroid. If a
        // non-convex validator rejects that path, finish toward the known
        // feasible best member instead.
        for (std::size_t repair = 0;
             !candidate.feasible && repair < 24 && problem_evaluator.can_evaluate(); ++repair) {
            const auto& target = repair < 12 ? centroid : complex.front().parameters;
            auto repaired = candidate.parameters;
            for (std::size_t parameter = 0; parameter < repaired.size(); ++parameter) {
                repaired[parameter] += (target[parameter] - repaired[parameter]) * 0.5;
            }
            repaired = normalize_design_unchecked(problem_definition, repaired);
            if (repaired == candidate.parameters) {
                break;
            }
            candidate = problem_evaluator.evaluate(repaired);
        }

        if (!rank_before(candidate, worst) && problem_evaluator.can_evaluate()) {
            std::vector<scalar> contracted(centroid.size());
            for (std::size_t parameter = 0; parameter < contracted.size(); ++parameter) {
                contracted[parameter] =
                    centroid[parameter] + options.contraction_factor *
                                              (worst.parameters[parameter] - centroid[parameter]);
            }
            candidate = problem_evaluator.evaluate(contracted);
            for (std::size_t repair = 0;
                 !candidate.feasible && repair < 24 && problem_evaluator.can_evaluate(); ++repair) {
                auto repaired = candidate.parameters;
                for (std::size_t parameter = 0; parameter < repaired.size(); ++parameter) {
                    repaired[parameter] +=
                        (complex.front().parameters[parameter] - repaired[parameter]) * 0.5;
                }
                repaired = normalize_design_unchecked(problem_definition, repaired);
                if (repaired == candidate.parameters) {
                    break;
                }
                candidate = problem_evaluator.evaluate(repaired);
            }
        }
        if (rank_before(candidate, worst)) {
            complex.back() = std::move(candidate);
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
        if (normalized_complex_diameter(problem_definition, complex) <= diameter_tolerance) {
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

    return finish_result(optimization_algorithm::box_method, problem_evaluator, iteration, reason);
}

} // namespace multobjopt::detail
