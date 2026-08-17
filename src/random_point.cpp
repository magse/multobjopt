#include "detail/optimizer_detail.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

/**
 * @file random_point.cpp
 * @brief Deterministic bounded point generation for stochastic optimizers.
 */

namespace multobjopt::detail {

/**
 * @brief Draw one bounded design using the supplied deterministic generator.
 * @param problem_definition Validated problem.
 * @param generator Pseudo-random engine owned by the calling optimizer.
 * @return A normalized design within every interval.
 *
 * Resolved parameters sample grid indices uniformly, rather than sampling a
 * real interval and rounding, which would give endpoint cells half the mass of
 * interior cells. Exceptionally large grids fall back to real sampling because
 * their index range cannot be represented by the distribution's integer type.
 */
std::vector<scalar> random_point(const problem& problem_definition, std::mt19937_64& generator) {
    std::vector<scalar> point;
    point.reserve(problem_definition.parameters().size());
    for (const auto& parameter : problem_definition.parameters()) {
        if (parameter.resolution > 0.0) {
            const long double ratio = (static_cast<long double>(parameter.upper_bound) -
                                       static_cast<long double>(parameter.lower_bound)) /
                                      static_cast<long double>(parameter.resolution);
            const long double slack =
                32.0L * static_cast<long double>(std::numeric_limits<scalar>::epsilon()) *
                std::max(1.0L, std::abs(ratio));
            const long double maximum_index = std::floor(ratio + slack);
            if (maximum_index <
                static_cast<long double>(std::numeric_limits<std::uint64_t>::max())) {
                std::uniform_int_distribution<std::uint64_t> distribution(
                    0, static_cast<std::uint64_t>(maximum_index));
                const auto index = distribution(generator);
                const long double value = static_cast<long double>(parameter.lower_bound) +
                                          static_cast<long double>(index) *
                                              static_cast<long double>(parameter.resolution);
                point.push_back(static_cast<scalar>(value));
                continue;
            }
        }
        std::uniform_real_distribution<scalar> distribution(parameter.lower_bound,
                                                            parameter.upper_bound);
        point.push_back(distribution(generator));
    }
    return normalize_design_unchecked(problem_definition, point);
}

} // namespace multobjopt::detail
