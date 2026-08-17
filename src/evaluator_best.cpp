#include "detail/optimizer_detail.hpp"

/**
 * @file evaluator_best.cpp
 * @brief Best-design query for the shared optimizer evaluator.
 */

namespace multobjopt::detail {

/**
 * @brief Return the feasibility-first scalar best design.
 * @return Reference to evaluator-owned result storage.
 * @pre evaluate() has completed successfully at least once.
 */
const evaluated_design& evaluator::best() const noexcept { return best_; }

} // namespace multobjopt::detail
