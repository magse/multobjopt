#include "detail/genetic_algorithm_detail.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

/**
 * @file population_metrics.cpp
 * @brief Nondomination rank and crowding-distance calculation.
 */

namespace multobjopt::detail {

/**
 * @brief Compute nondomination rank and crowding distance for a population.
 * @param problem_definition Problem supplying objective senses and dominance.
 * @param population Fully evaluated current population.
 * @return One selection metric for each population element in input order.
 *
 * Fast nondominated sorting constructs fronts from pairwise dominance. For
 * every objective, the two boundary members receive infinite crowding and
 * interior members receive normalized adjacent spacing. Non-finite objective
 * values are sorted last and are excluded from finite spacing arithmetic,
 * preventing NaN values from corrupting selection order.
 */
std::vector<population_metric> population_metrics(const problem& problem_definition,
                                                  const std::vector<evaluated_design>& population) {
    const std::size_t count = population.size();
    std::vector<std::vector<std::size_t>> dominated(count);
    std::vector<std::size_t> domination_count(count, 0);
    std::vector<std::vector<std::size_t>> fronts(1);

    for (std::size_t lhs = 0; lhs < count; ++lhs) {
        for (std::size_t rhs = lhs + 1; rhs < count; ++rhs) {
            if (dominates(problem_definition, population[lhs], population[rhs])) {
                dominated[lhs].push_back(rhs);
                ++domination_count[rhs];
            } else if (dominates(problem_definition, population[rhs], population[lhs])) {
                dominated[rhs].push_back(lhs);
                ++domination_count[lhs];
            }
        }
    }

    for (std::size_t index = 0; index < count; ++index) {
        if (domination_count[index] == 0) {
            fronts.front().push_back(index);
        }
    }

    std::vector<population_metric> metrics(count);
    std::size_t front_index = 0;
    while (front_index < fronts.size() && !fronts[front_index].empty()) {
        std::vector<std::size_t> next;
        for (const auto member : fronts[front_index]) {
            metrics[member].rank = front_index;
            for (const auto other : dominated[member]) {
                if (--domination_count[other] == 0) {
                    next.push_back(other);
                }
            }
        }
        if (!next.empty()) {
            fronts.push_back(std::move(next));
        }
        ++front_index;
    }

    for (const auto& front : fronts) {
        if (front.empty()) {
            continue;
        }
        if (front.size() <= 2) {
            for (const auto member : front) {
                metrics[member].crowding = std::numeric_limits<scalar>::infinity();
            }
            continue;
        }

        for (std::size_t objective = 0; objective < problem_definition.objectives().size();
             ++objective) {
            auto order = front;
            std::sort(order.begin(), order.end(), [&](std::size_t lhs, std::size_t rhs) {
                const scalar lhs_value =
                    sortable_objective(problem_definition.objectives()[objective],
                                       population[lhs].objectives[objective]);
                const scalar rhs_value =
                    sortable_objective(problem_definition.objectives()[objective],
                                       population[rhs].objectives[objective]);
                if (lhs_value != rhs_value) {
                    return lhs_value < rhs_value;
                }
                return lhs < rhs;
            });
            metrics[order.front()].crowding = std::numeric_limits<scalar>::infinity();
            metrics[order.back()].crowding = std::numeric_limits<scalar>::infinity();
            const scalar minimum =
                sortable_objective(problem_definition.objectives()[objective],
                                   population[order.front()].objectives[objective]);
            const scalar maximum =
                sortable_objective(problem_definition.objectives()[objective],
                                   population[order.back()].objectives[objective]);
            if (!finite(minimum) || !finite(maximum) || maximum == minimum) {
                continue;
            }
            for (std::size_t position = 1; position + 1 < order.size(); ++position) {
                if (!finite(metrics[order[position]].crowding)) {
                    continue;
                }
                const scalar before =
                    sortable_objective(problem_definition.objectives()[objective],
                                       population[order[position - 1]].objectives[objective]);
                const scalar after =
                    sortable_objective(problem_definition.objectives()[objective],
                                       population[order[position + 1]].objectives[objective]);
                if (!finite(before) || !finite(after)) {
                    continue;
                }
                const long double distance =
                    (static_cast<long double>(after) - static_cast<long double>(before)) /
                    (static_cast<long double>(maximum) - static_cast<long double>(minimum));
                if (std::isfinite(distance)) {
                    metrics[order[position]].crowding += static_cast<scalar>(distance);
                }
            }
        }
    }
    return metrics;
}

} // namespace multobjopt::detail
