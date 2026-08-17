#include "detail/genetic_algorithm_detail.hpp"

/**
 * @file metric_before.cpp
 * @brief Stable rank-and-crowding comparison for genetic selection.
 */

namespace multobjopt::detail {

/**
 * @brief Compare population indices using rank, diversity, and stable fallback.
 * @param lhs First population index.
 * @param rhs Second population index.
 * @param population Evaluated population addressed by the indices.
 * @param metrics Rank and crowding data for @p population.
 * @return True when @p lhs should be selected before @p rhs.
 *
 * The shared feasibility-first ordering is the final tie-breaker. It includes
 * lexicographic parameters, which makes elite sorting and tournaments stable
 * for a fixed seed even when rank and crowding are equal.
 */
bool metric_before(std::size_t lhs, std::size_t rhs,
                   const std::vector<evaluated_design>& population,
                   const std::vector<population_metric>& metrics) noexcept {
    if (metrics[lhs].rank != metrics[rhs].rank) {
        return metrics[lhs].rank < metrics[rhs].rank;
    }
    if (metrics[lhs].crowding != metrics[rhs].crowding) {
        return metrics[lhs].crowding > metrics[rhs].crowding;
    }
    return rank_before(population[lhs], population[rhs]);
}

} // namespace multobjopt::detail
