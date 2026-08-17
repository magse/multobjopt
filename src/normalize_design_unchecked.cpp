#include "detail/optimizer_detail.hpp"

#include <stdexcept>
#include <vector>

/**
 * @file normalize_design_unchecked.cpp
 * @brief Internal projection of a design for an already validated problem.
 *
 * Parameter grids are anchored at the lower bound. A non-aligned upper bound
 * remains a bound and is not introduced as an additional grid point.
 */

namespace multobjopt::detail {

/**
 * @brief Project every design component after problem validation.
 * @param problem_definition Validated problem supplying bounds and grids.
 * @param parameters Proposed design vector.
 * @return A normalized vector with the same component order.
 * @throws std::invalid_argument if the vector dimension is incorrect or any
 *         component is not finite.
 */
std::vector<scalar> normalize_design_unchecked(const problem& problem_definition,
                                               scalar_view parameters) {
    if (parameters.size() != problem_definition.parameters().size()) {
        throw std::invalid_argument("design vector size does not match the problem");
    }
    std::vector<scalar> result;
    result.reserve(parameters.size());
    for (std::size_t index = 0; index < parameters.size(); ++index) {
        result.push_back(
            normalize_parameter(problem_definition.parameters()[index], parameters[index]));
    }
    return result;
}

} // namespace multobjopt::detail
