#include "detail/optimizer_detail.hpp"

/**
 * @file evaluator_evaluation_history.cpp
 * @brief Chronological completed-evaluation history query.
 */

namespace multobjopt::detail {

/**
 * @brief Return the optional chronological design-evaluation history.
 * @return Records in exact successful evaluate() call order, or an empty
 *         vector when optimizer_options::record_evaluation_history is false.
 *
 * Repeated designs remain distinct entries because this trajectory describes
 * completed work rather than the filtered Pareto archive. Callback exceptions
 * propagate before a completed design exists and therefore create no entry.
 */
const std::vector<evaluated_design>& evaluator::evaluation_history() const noexcept {
    return evaluation_history_;
}

} // namespace multobjopt::detail
