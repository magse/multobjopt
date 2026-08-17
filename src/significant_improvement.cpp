#include "detail/optimizer_detail.hpp"

/**
 * @file significant_improvement.cpp
 * @brief Internal optimizer-stall progress comparison.
 */

namespace multobjopt::detail {

/**
 * @brief Determine whether best-design progress is large enough to reset stalling.
 * @param current Current best design.
 * @param previous Best design before the last outer iteration.
 * @param tolerance Required reduction in violation or scalar compromise.
 * @return True for a new feasible design or a reduction exceeding @p tolerance.
 */
bool significant_improvement(const evaluated_design& current, const evaluated_design& previous,
                             scalar tolerance) noexcept {
    if (current.feasible != previous.feasible) {
        return current.feasible;
    }
    if (!current.feasible) {
        return previous.total_violation - current.total_violation > tolerance;
    }
    return previous.scalarized_objective - current.scalarized_objective > tolerance;
}

} // namespace multobjopt::detail
