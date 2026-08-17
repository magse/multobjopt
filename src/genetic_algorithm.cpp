#include "detail/optimizer_detail.hpp"

#include "detail/genetic_algorithm_detail.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

/**
 * @file genetic_algorithm.cpp
 * @brief Pareto-aware genetic search with elitism and crowding preservation.
 *
 * The population is ordered with nondominated sorting and NSGA-II-style
 * crowding distances. Constraint handling remains consistent with the rest of
 * the library because public dominance ranks feasible designs before
 * infeasible designs and compares infeasible candidates by total violation.
 * Tournament selection, arithmetic crossover, Gaussian mutation, and elitism
 * then build each generation deterministically for a fixed random seed.
 *
 * All offspring evaluations pass through detail::evaluator. This maintains the
 * feasible nondominated archive and guarantees that incomplete generations do
 * not overrun the global evaluation budget.
 */

namespace multobjopt::detail {

/**
 * @brief Optimize a problem with a Pareto-aware genetic algorithm.
 * @param problem_definition Validated optimization problem.
 * @param options Validated common optimizer options.
 * @return Best feasibility-first design, feasible Pareto archive, accounting
 *         totals, and the reason evolution stopped.
 *
 * The initial guess is retained as the first individual and the remainder of
 * the initial population is sampled across the bounded design space. Each
 * generation carries forward configured elites, chooses parents by tournament,
 * blends coordinates during crossover, and applies range-scaled Gaussian
 * mutation. Discrete coordinates are moved by at least one resolution interval
 * whenever mutation is selected, after which the evaluator projects them to
 * their lower-bound-anchored lattice.
 *
 * A generation counts as progress if either the Pareto archive changes or the
 * feasibility-first scalar best improves beyond the configured tolerance.
 * Population construction checks the budget before every child, so a partial
 * final generation is valid and terminates with the evaluation-limit reason.
 */
optimization_result run_genetic_algorithm(const problem& problem_definition,
                                          const optimizer_options& options) {
    evaluator problem_evaluator(problem_definition, options);
    std::mt19937_64 generator(options.random_seed);
    std::uniform_real_distribution<scalar> unit(0.0, 1.0);
    std::normal_distribution<scalar> normal(0.0, 1.0);

    std::vector<evaluated_design> population;
    population.reserve(options.population_size);
    population.push_back(problem_evaluator.evaluate(starting_point(problem_definition, options)));
    const std::size_t free_count = free_parameter_count(problem_definition);
    if (free_count == 0) {
        return finish_result(optimization_algorithm::genetic_algorithm, problem_evaluator, 0,
                             termination_reason::converged);
    }
    while (population.size() < options.population_size && problem_evaluator.can_evaluate()) {
        population.push_back(
            problem_evaluator.evaluate(random_point(problem_definition, generator)));
    }

    if (population.size() == 1) {
        return finish_result(optimization_algorithm::genetic_algorithm, problem_evaluator, 0,
                             problem_evaluator.can_evaluate()
                                 ? termination_reason::converged
                                 : termination_reason::evaluation_limit);
    }
    if (!problem_evaluator.can_evaluate()) {
        return finish_result(optimization_algorithm::genetic_algorithm, problem_evaluator, 0,
                             termination_reason::evaluation_limit);
    }

    std::size_t iteration = 0;
    std::size_t stalled = 0;
    termination_reason reason = termination_reason::iteration_limit;
    for (; iteration < options.max_iterations && problem_evaluator.can_evaluate(); ++iteration) {
        const auto previous_best = problem_evaluator.best();
        const std::size_t previous_archive_revision = problem_evaluator.archive_revision();
        const auto metrics = population_metrics(problem_definition, population);
        std::vector<std::size_t> order(population.size());
        std::iota(order.begin(), order.end(), std::size_t{0});
        std::sort(order.begin(), order.end(), [&](std::size_t lhs, std::size_t rhs) {
            return metric_before(lhs, rhs, population, metrics);
        });

        std::vector<evaluated_design> next_population;
        next_population.reserve(options.population_size);
        const std::size_t elite_count =
            std::min({options.elite_count, options.population_size, population.size()});
        for (std::size_t elite = 0; elite < elite_count; ++elite) {
            next_population.push_back(population[order[elite]]);
        }

        while (next_population.size() < options.population_size &&
               problem_evaluator.can_evaluate()) {
            const auto& parent_a = population[tournament_select(
                population, metrics, options.tournament_size, generator)];
            const auto& parent_b = population[tournament_select(
                population, metrics, options.tournament_size, generator)];
            std::vector<scalar> child = parent_a.parameters;

            for (std::size_t index = 0; index < child.size(); ++index) {
                const auto& parameter = problem_definition.parameters()[index];
                const scalar range = parameter.upper_bound - parameter.lower_bound;
                if (range == 0.0) {
                    continue;
                }
                if (unit(generator) < options.crossover_rate) {
                    const scalar blend = unit(generator);
                    child[index] =
                        std::lerp(parent_b.parameters[index], parent_a.parameters[index], blend);
                }
                if (unit(generator) < options.mutation_rate) {
                    long double mutation = static_cast<long double>(normal(generator)) *
                                           static_cast<long double>(options.mutation_scale) *
                                           static_cast<long double>(range);
                    if (parameter.resolution > 0.0) {
                        if (std::abs(mutation) < static_cast<long double>(parameter.resolution)) {
                            mutation = (unit(generator) < 0.5 ? -1.0L : 1.0L) *
                                       static_cast<long double>(parameter.resolution);
                        }
                    }
                    const long double mutated =
                        std::clamp(static_cast<long double>(child[index]) + mutation,
                                   static_cast<long double>(parameter.lower_bound),
                                   static_cast<long double>(parameter.upper_bound));
                    child[index] = static_cast<scalar>(mutated);
                }
            }
            next_population.push_back(problem_evaluator.evaluate(child));
        }
        population = std::move(next_population);

        if (problem_evaluator.archive_revision() != previous_archive_revision ||
            significant_improvement(problem_evaluator.best(), previous_best,
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
    }

    return finish_result(optimization_algorithm::genetic_algorithm, problem_evaluator, iteration,
                         reason);
}

} // namespace multobjopt::detail
