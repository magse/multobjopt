#include "detail/optimizer_detail.hpp"

#include <cmath>

/**
 * @file finite.cpp
 * @brief Internal finite-scalar classification.
 */

namespace multobjopt::detail {

/**
 * @brief Test whether a scalar is a usable finite value.
 * @param value Value produced by configuration or user model code.
 * @return True only for values other than positive infinity, negative infinity,
 *         and NaN.
 */
bool finite(scalar value) noexcept { return std::isfinite(value); }

} // namespace multobjopt::detail
