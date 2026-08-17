#include "detail/genetic_algorithm_detail.hpp"

#include <random>

/**
 * @file tournament_select.cpp
 * @brief Parent tournament operation for the genetic optimizer.
 */

namespace multobjopt::detail {

/**
 * @brief Select one parent through repeated rank-and-crowding tournaments.
 * @param population Current evaluated population.
 * @param metrics Selection metrics corresponding to @p population.
 * @param tournament_size Number of random competitors, including the first.
 * @param generator Pseudorandom generator whose draw sequence is preserved.
 * @return Index of the winning population member.
 */
std::size_t tournament_select(const std::vector<evaluated_design>& population,
                              const std::vector<population_metric>& metrics,
                              std::size_t tournament_size, std::mt19937_64& generator) {
    std::uniform_int_distribution<std::size_t> index_distribution(0, population.size() - 1);
    std::size_t winner = index_distribution(generator);
    for (std::size_t draw = 1; draw < tournament_size; ++draw) {
        const std::size_t challenger = index_distribution(generator);
        if (metric_before(challenger, winner, population, metrics)) {
            winner = challenger;
        }
    }
    return winner;
}

} // namespace multobjopt::detail
