#include "detail/optimizer_detail.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

/**
 * @file normalize_parameter.cpp
 * @brief Internal projection of one parameter onto its bounded resolution grid.
 */

namespace multobjopt::detail {

/**
 * @brief Clamp one parameter and, when resolved, snap it to its lower-bound grid.
 * @param definition Validated parameter definition.
 * @param value Proposed scalar value.
 * @return The nearest admitted value inside the inclusive bounds.
 * @throws std::invalid_argument if @p value is not finite.
 *
 * Grid indices are computed in long-double arithmetic to limit cancellation
 * for translated intervals. The tolerance remains based on `scalar` epsilon
 * because all public inputs originate as scalar values. The maximum index is
 * floored, so an upper bound that is not grid-aligned is deliberately excluded.
 */
scalar normalize_parameter(const parameter_definition& definition, scalar value) {
    if (!finite(value)) {
        throw std::invalid_argument("design parameter values must be finite");
    }

    value = std::clamp(value, definition.lower_bound, definition.upper_bound);
    if (definition.resolution == 0.0 || definition.lower_bound == definition.upper_bound) {
        return value;
    }

    const auto lower = static_cast<long double>(definition.lower_bound);
    const auto upper = static_cast<long double>(definition.upper_bound);
    const auto resolution = static_cast<long double>(definition.resolution);
    const auto q = (static_cast<long double>(value) - lower) / resolution;
    const auto q_max = (upper - lower) / resolution;
    // Inputs originate as double values, so the grid-index tolerance must use
    // double precision even though the arithmetic itself uses long double.
    const auto rounding_slack = 32.0L *
                                static_cast<long double>(std::numeric_limits<scalar>::epsilon()) *
                                std::max(1.0L, std::abs(q_max));
    const auto maximum_index = std::max(0.0L, std::floor(q_max + rounding_slack));
    const auto index = std::clamp(std::floor(q + 0.5L), 0.0L, maximum_index);
    const auto snapped = lower + index * resolution;
    return static_cast<scalar>(std::min(snapped, upper));
}

} // namespace multobjopt::detail
