#include "detail/optimizer_detail.hpp"

#include <cmath>

/**
 * @file equal_objectives.cpp
 * @brief Internal tolerance comparison of complete objective vectors.
 */

namespace multobjopt::detail {

/**
 * @brief Compare complete raw objective vectors within an absolute tolerance.
 * @param lhs First evaluated design.
 * @param rhs Second evaluated design.
 * @param tolerance Maximum accepted component difference.
 * @return True when dimensions match and no component differs by more than the
 *         tolerance.
 */
bool equal_objectives(const evaluated_design& lhs, const evaluated_design& rhs,
                      scalar tolerance) noexcept {
    if (lhs.objectives.size() != rhs.objectives.size()) {
        return false;
    }
    for (std::size_t index = 0; index < lhs.objectives.size(); ++index) {
        if (std::abs(lhs.objectives[index] - rhs.objectives[index]) > tolerance) {
            return false;
        }
    }
    return true;
}

} // namespace multobjopt::detail
