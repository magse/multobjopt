#include "detail/optimizer_detail.hpp"

#include <stdexcept>

/**
 * @file evaluator_evaluate.cpp
 * @brief Budgeted evaluation and shared best-design tracking.
 */

namespace multobjopt::detail {

/**
 * @brief Normalize, evaluate, rank, and archive one proposed design.
 * @param parameters Proposed parameter vector.
 * @return The complete evaluated design.
 * @throws std::logic_error if the caller has exhausted the configured budget.
 *
 * The accounting unit is one complete design evaluation, independent of the
 * number of objective and restriction callbacks. The counter advances exactly
 * once after all callbacks return normally.
 */
evaluated_design evaluator::evaluate(scalar_view parameters) {
    if (!can_evaluate()) {
        throw std::logic_error("internal evaluation budget overrun");
    }

    auto result = evaluate_unchecked(problem_, parameters, options_.constraint_tolerance);
    ++evaluations_;

    if (!has_best_ || rank_before(result, best_)) {
        best_ = result;
        has_best_ = true;
    }
    update_archive(result);
    return result;
}

} // namespace multobjopt::detail
