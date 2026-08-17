#include "detail/optimizer_detail.hpp"

/**
 * @file evaluator_can_evaluate.cpp
 * @brief Evaluation-budget availability query for the shared evaluator.
 */

namespace multobjopt::detail {

/**
 * @brief Report whether one more complete design evaluation is permitted.
 * @return True while the exact count is below `max_evaluations`.
 */
bool evaluator::can_evaluate() const noexcept { return evaluations_ < options_.max_evaluations; }

} // namespace multobjopt::detail
