#ifndef MULTOBJOPT_DETAIL_GRADIENT_DESCENT_DETAIL_HPP
#define MULTOBJOPT_DETAIL_GRADIENT_DESCENT_DETAIL_HPP

#include "optimizer_detail.hpp"

/**
 * @file gradient_descent_detail.hpp
 * @brief Internal merit operation used by projected gradient descent.
 */

namespace multobjopt::detail {

/** @brief Convert an evaluated design to the penalized descent merit. */
[[nodiscard]] scalar gradient_merit(const evaluated_design& design, scalar penalty_factor) noexcept;

} // namespace multobjopt::detail

#endif
