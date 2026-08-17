#include "detail/optimizer_detail.hpp"

#include <numeric>

/**
 * @file midpoint.cpp
 * @brief Overflow-safe midpoint construction for bounded designs.
 */

namespace multobjopt::detail {

/**
 * @brief Construct the normalized midpoint of all parameter intervals.
 * @param problem_definition Validated problem.
 * @return Midpoint design projected onto every configured grid.
 *
 * `std::midpoint` avoids overflow that a direct `(lower + upper) / 2` can incur
 * for large, same-sign finite bounds.
 */
std::vector<scalar> midpoint(const problem& problem_definition) {
    std::vector<scalar> point;
    point.reserve(problem_definition.parameters().size());
    for (const auto& parameter : problem_definition.parameters()) {
        point.push_back(std::midpoint(parameter.lower_bound, parameter.upper_bound));
    }
    return normalize_design_unchecked(problem_definition, point);
}

} // namespace multobjopt::detail
