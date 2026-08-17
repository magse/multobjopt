#include "detail/optimizer_detail.hpp"

/**
 * @file evaluator_evaluations.cpp
 * @brief Evaluation-count query for the shared optimizer evaluator.
 */

namespace multobjopt::detail {

/**
 * @brief Return the number of complete designs evaluated so far.
 * @return Exact count of complete design evaluations.
 */
std::size_t evaluator::evaluations() const noexcept { return evaluations_; }

} // namespace multobjopt::detail
