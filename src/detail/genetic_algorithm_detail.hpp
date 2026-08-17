#ifndef MULTOBJOPT_DETAIL_GENETIC_ALGORITHM_DETAIL_HPP
#define MULTOBJOPT_DETAIL_GENETIC_ALGORITHM_DETAIL_HPP

#include "optimizer_detail.hpp"

#include <cstddef>
#include <random>
#include <vector>

/**
 * @file genetic_algorithm_detail.hpp
 * @brief Internal selection operations used by the genetic optimizer.
 */

namespace multobjopt::detail {

/**
 * @brief Selection metadata associated with one population member.
 *
 * Smaller ranks are better. Within one front, a larger crowding value is
 * preferred because it represents a less densely sampled part of objective
 * space.
 */
struct population_metric {
    /** @brief Zero-based nondomination-front index. */
    std::size_t rank{0};

    /** @brief Normalized neighbor distance accumulated across objectives. */
    scalar crowding{0.0};
};

/** @brief Compute nondomination rank and crowding distance for a population. */
[[nodiscard]] std::vector<population_metric>
population_metrics(const problem& problem_definition,
                   const std::vector<evaluated_design>& population);

/** @brief Compare population indices using rank, diversity, and stable fallback. */
[[nodiscard]] bool metric_before(std::size_t lhs, std::size_t rhs,
                                 const std::vector<evaluated_design>& population,
                                 const std::vector<population_metric>& metrics) noexcept;

/** @brief Select one parent through repeated rank-and-crowding tournaments. */
[[nodiscard]] std::size_t tournament_select(const std::vector<evaluated_design>& population,
                                            const std::vector<population_metric>& metrics,
                                            std::size_t tournament_size,
                                            std::mt19937_64& generator);

} // namespace multobjopt::detail

#endif
