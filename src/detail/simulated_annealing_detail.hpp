#ifndef MULTOBJOPT_DETAIL_SIMULATED_ANNEALING_DETAIL_HPP
#define MULTOBJOPT_DETAIL_SIMULATED_ANNEALING_DETAIL_HPP

#include "optimizer_detail.hpp"

/**
 * @file simulated_annealing_detail.hpp
 * @brief Internal acceptance-energy operation used by simulated annealing.
 */

namespace multobjopt::detail {

/** @brief Compute the non-negative annealing energy needed for a worse move. */
[[nodiscard]] scalar annealing_worsening_delta(const evaluated_design& candidate,
                                               const evaluated_design& current,
                                               scalar penalty_factor) noexcept;

} // namespace multobjopt::detail

#endif
