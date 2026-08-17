#include "detail/optimizer_detail.hpp"

#include <algorithm>

/**
 * @file select_algorithm.cpp
 * @brief Public automatic optimizer selection from observable problem structure.
 */

namespace multobjopt {

/**
 * @brief Select an optimizer using only observable problem structure.
 * @param problem_definition Problem whose validated structure controls selection.
 * @return Algorithm selected by the documented structural heuristic.
 *
 * User callbacks are opaque, so this function does not attempt to infer
 * smoothness or noise. Multi-objective and resolved problems favor population
 * search, constrained continuous problems favor a feasible Box complex, small
 * scalar continuous problems favor numerical gradients, and larger scalar
 * continuous problems favor annealing.
 */
optimization_algorithm select_algorithm(const problem& problem_definition) {
    problem_definition.validate();
    if (problem_definition.objectives().size() > 1 ||
        std::any_of(problem_definition.parameters().begin(), problem_definition.parameters().end(),
                    [](const auto& parameter) { return parameter.resolution > 0.0; })) {
        return optimization_algorithm::genetic_algorithm;
    }
    if (!problem_definition.restrictions().empty() ||
        static_cast<bool>(problem_definition.validation())) {
        return optimization_algorithm::box_method;
    }
    if (detail::free_parameter_count(problem_definition) <= 12) {
        return optimization_algorithm::gradient_descent;
    }
    return optimization_algorithm::simulated_annealing;
}

} // namespace multobjopt
