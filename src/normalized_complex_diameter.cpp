#include "detail/box_method_detail.hpp"

#include <algorithm>

/**
 * @file normalized_complex_diameter.cpp
 * @brief Normalized convergence measurement for the Box complex.
 */

namespace multobjopt::detail {

/**
 * @brief Measure the largest coordinate spread of a Box complex.
 *
 * Fixed coordinates are ignored to avoid division by zero. Normalizing each
 * coordinate by its range makes the convergence test insensitive to engineering
 * units and to large differences in parameter scale.
 */
scalar normalized_complex_diameter(const problem& problem_definition,
                                   const std::vector<evaluated_design>& complex) noexcept {
    scalar diameter = 0.0;
    for (std::size_t parameter_index = 0; parameter_index < problem_definition.parameters().size();
         ++parameter_index) {
        const auto& parameter = problem_definition.parameters()[parameter_index];
        const scalar range = parameter.upper_bound - parameter.lower_bound;
        if (range == 0.0) {
            continue;
        }
        auto [minimum, maximum] = std::minmax_element(
            complex.begin(), complex.end(), [&](const auto& lhs, const auto& rhs) {
                return lhs.parameters[parameter_index] < rhs.parameters[parameter_index];
            });
        diameter = std::max(
            diameter,
            (maximum->parameters[parameter_index] - minimum->parameters[parameter_index]) / range);
    }
    return diameter;
}

} // namespace multobjopt::detail
