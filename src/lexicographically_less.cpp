#include "detail/optimizer_detail.hpp"

#include <algorithm>

/**
 * @file lexicographically_less.cpp
 * @brief Internal deterministic ordering of scalar vectors.
 */

namespace multobjopt::detail {

/**
 * @brief Compare scalar vectors lexicographically for deterministic tie-breaking.
 * @param lhs Left vector.
 * @param rhs Right vector.
 * @return True when @p lhs precedes @p rhs lexicographically.
 */
bool lexicographically_less(scalar_view lhs, scalar_view rhs) noexcept {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

} // namespace multobjopt::detail
