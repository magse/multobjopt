#include "detail/optimizer_detail.hpp"

#include <algorithm>

/**
 * @file free_parameter_count.cpp
 * @brief Count of independently variable problem dimensions.
 */

namespace multobjopt::detail {

/**
 * @brief Count parameter dimensions that can change.
 * @param problem_definition Validated problem.
 * @return Number of definitions whose lower and upper bounds differ.
 */
std::size_t free_parameter_count(const problem& problem_definition) {
    return static_cast<std::size_t>(std::count_if(
        problem_definition.parameters().begin(), problem_definition.parameters().end(),
        [](const auto& parameter) { return parameter.lower_bound != parameter.upper_bound; }));
}

} // namespace multobjopt::detail
