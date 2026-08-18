#include "detail/reporting_detail.hpp"

#include <array>
#include <charconv>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

/**
 * @file reporting_format.cpp
 * @brief Locale-independent scalar and index formatting for reports.
 */

namespace multobjopt::detail {

/**
 * @brief Format a scalar for round-trip tabular interchange.
 *
 * to_chars avoids inheriting a caller's locale or stream precision. The three
 * non-finite tokens are deliberately lower-case and stable across platforms.
 */
std::string format_report_scalar(scalar value) {
    if (std::isnan(value)) {
        return "nan";
    }
    if (std::isinf(value)) {
        return std::signbit(value) ? "-inf" : "inf";
    }

    std::array<char, 128> buffer{};
    const auto [end, error] =
        std::to_chars(buffer.data(), buffer.data() + buffer.size(), value,
                      std::chars_format::general, std::numeric_limits<scalar>::max_digits10);
    if (error != std::errc{}) {
        throw std::runtime_error("failed to format report scalar");
    }
    return {buffer.data(), end};
}

/** @brief Format a one-based row index without locale-dependent grouping. */
std::string format_report_index(std::size_t value) {
    std::array<char, std::numeric_limits<std::size_t>::digits10 + 3> buffer{};
    const auto [end, error] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
    if (error != std::errc{}) {
        throw std::runtime_error("failed to format report index");
    }
    return {buffer.data(), end};
}

} // namespace multobjopt::detail
