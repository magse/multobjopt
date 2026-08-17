#include "detail/optimizer_detail.hpp"

#include "detail/simulated_annealing_detail.hpp"

#include <algorithm>
#include <cmath>
#include <random>

/**
 * @file simulated_annealing.cpp
 * @brief Temperature-controlled stochastic search for bounded designs.
 *
 * The implementation perturbs every free parameter, projects the proposal to
 * the parameter bounds and resolution grid through the shared evaluator, and
 * accepts either an improvement or a probabilistically selected worsening
 * move. Feasibility is part of the acceptance energy: feasible designs always
 * outrank infeasible ones, while constraint violation supplies the energy
 * difference between infeasible designs.
 *
 * Evaluation accounting is centralized in detail::evaluator. Consequently,
 * every proposal consumes exactly one complete design evaluation and the
 * inner loop stops before the configured budget can be exceeded.
 */

namespace multobjopt::detail {
/**
 * @brief Optimize a problem with bounded simulated annealing.
 * @param problem_definition Validated optimization problem.
 * @param options Validated common optimizer options.
 * @return Best feasibility-first design, feasible Pareto archive, accounting
 *         totals, and the reason the temperature search stopped.
 *
 * A Gaussian move is scaled by both the parameter range and the square root
 * of the current temperature fraction. For a discretized parameter, a sampled
 * move smaller than one grid interval is promoted to exactly one interval so
 * that cooling does not prematurely freeze that coordinate. The evaluator
 * then performs the definitive bound and lattice projection.
 *
 * Better designs are always accepted. Worse designs use the Metropolis
 * probability `exp(-delta / temperature)`, where @c delta observes the
 * feasibility-first ordering. Convergence is reported when cooling reaches
 * the final temperature; lack of meaningful best-design improvement is
 * reported as a stall, and a depleted evaluation budget takes precedence
 * inside an iteration.
 */
[[nodiscard]] optimization_result run_simulated_annealing(const problem& problem_definition,
                                                          const optimizer_options& options) {
    evaluator problem_evaluator(problem_definition, options);
    std::mt19937_64 generator(options.random_seed);
    std::uniform_real_distribution<scalar> unit(0.0, 1.0);
    std::normal_distribution<scalar> normal(0.0, 1.0);

    auto current = problem_evaluator.evaluate(starting_point(problem_definition, options));
    if (free_parameter_count(problem_definition) == 0) {
        return finish_result(optimization_algorithm::simulated_annealing, problem_evaluator, 0,
                             termination_reason::converged);
    }

    scalar temperature = options.initial_temperature;
    const std::size_t moves =
        options.moves_per_temperature == 0
            ? std::max<std::size_t>(10, 4 * problem_definition.parameters().size())
            : options.moves_per_temperature;
    std::size_t iteration = 0;
    std::size_t stalled = 0;
    termination_reason reason = termination_reason::iteration_limit;

    for (; iteration < options.max_iterations && temperature > options.final_temperature;
         ++iteration) {
        const auto previous_best = problem_evaluator.best();
        for (std::size_t move = 0; move < moves && problem_evaluator.can_evaluate(); ++move) {
            auto proposal = current.parameters;
            const scalar temperature_fraction =
                std::max(temperature / options.initial_temperature,
                         options.final_temperature / options.initial_temperature);

            for (std::size_t index = 0; index < proposal.size(); ++index) {
                const auto& parameter = problem_definition.parameters()[index];
                const scalar range = parameter.upper_bound - parameter.lower_bound;
                if (range == 0.0) {
                    continue;
                }
                long double step = static_cast<long double>(range) *
                                   static_cast<long double>(options.annealing_step_scale) *
                                   static_cast<long double>(std::sqrt(temperature_fraction)) *
                                   static_cast<long double>(normal(generator));
                if (parameter.resolution > 0.0 && std::abs(step) < parameter.resolution) {
                    step = (normal(generator) < 0.0 ? -1.0 : 1.0) * parameter.resolution;
                }
                proposal[index] = static_cast<scalar>(
                    std::clamp(static_cast<long double>(proposal[index]) + step,
                               static_cast<long double>(parameter.lower_bound),
                               static_cast<long double>(parameter.upper_bound)));
            }

            const auto candidate = problem_evaluator.evaluate(proposal);
            bool accept = rank_before(candidate, current);
            if (!accept) {
                const scalar delta =
                    annealing_worsening_delta(candidate, current, options.penalty_factor);
                accept = finite(delta) && unit(generator) < std::exp(-delta / temperature);
            }
            if (accept) {
                current = candidate;
            }
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
        if (stalled >= options.stall_iterations) {
            reason = termination_reason::stalled;
            ++iteration;
            break;
        }
        temperature *= options.cooling_rate;
    }

    if (temperature <= options.final_temperature) {
        reason = termination_reason::converged;
    } else if (iteration >= options.max_iterations &&
               reason == termination_reason::iteration_limit) {
        reason = termination_reason::iteration_limit;
    }
    return finish_result(optimization_algorithm::simulated_annealing, problem_evaluator, iteration,
                         reason);
}

} // namespace multobjopt::detail
