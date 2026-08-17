#include "detail/optimizer_detail.hpp"

/**
 * @file evaluator_archive_revision.cpp
 * @brief Pareto-archive revision query for the shared evaluator.
 */

namespace multobjopt::detail {

/**
 * @brief Return the number of changes made to the nondominated archive.
 * @return Monotonic revision counter used for algorithm stall detection.
 */
std::size_t evaluator::archive_revision() const noexcept { return archive_revision_; }

} // namespace multobjopt::detail
