#ifndef MULTOBJOPT_DETAIL_BOX_METHOD_DETAIL_HPP
#define MULTOBJOPT_DETAIL_BOX_METHOD_DETAIL_HPP

#include "optimizer_detail.hpp"

#include <vector>

/**
 * @file box_method_detail.hpp
 * @brief Internal support operation used by the Box optimizer.
 */

namespace multobjopt::detail {

/**
 * @brief Measure the largest normalized coordinate spread of a Box complex.
 * @param problem_definition Problem defining the coordinate ranges.
 * @param complex Feasible set maintained by the Box iteration.
 * @return Largest member spread divided by the corresponding parameter range.
 */
[[nodiscard]] scalar
normalized_complex_diameter(const problem& problem_definition,
                            const std::vector<evaluated_design>& complex) noexcept;

} // namespace multobjopt::detail

#endif
