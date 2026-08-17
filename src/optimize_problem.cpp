#include "detail/optimizer_detail.hpp"

#include <stdexcept>

/**
 * @file optimize_problem.cpp
 * @brief Public validation and dispatch entry point for optimization.
 */

namespace multobjopt {

/**
 * @brief Validate and solve a problem with one common options object.
 * @param problem_definition User-defined optimization problem.
 * @param options Common controls for selection and the chosen optimizer.
 * @return Optimization result produced by the selected algorithm.
 *
 * The requested algorithm is retained verbatim unless it is `automatic`, in
 * which case select_algorithm() resolves it before dispatch. Initial guesses
 * may lie outside the bounds because each optimizer normalizes them, but every
 * supplied component must be finite and the vector size must match exactly.
 */
optimization_result optimize(const problem& problem_definition, const optimizer_options& options) {
    problem_definition.validate();
    options.validate();
    if (!options.initial_guess.empty()) {
        if (options.initial_guess.size() != problem_definition.parameters().size()) {
            throw std::invalid_argument("initial_guess size does not match the problem");
        }
        for (const auto value : options.initial_guess) {
            if (!detail::finite(value)) {
                throw std::invalid_argument("initial_guess values must be finite");
            }
        }
    }

    const optimization_algorithm selected = options.algorithm == optimization_algorithm::automatic
                                                ? select_algorithm(problem_definition)
                                                : options.algorithm;
    switch (selected) {
    case optimization_algorithm::simulated_annealing:
        return detail::run_simulated_annealing(problem_definition, options);
    case optimization_algorithm::genetic_algorithm:
        return detail::run_genetic_algorithm(problem_definition, options);
    case optimization_algorithm::box_method:
        return detail::run_box_method(problem_definition, options);
    case optimization_algorithm::gradient_descent:
        return detail::run_gradient_descent(problem_definition, options);
    case optimization_algorithm::automatic:
        break;
    }
    throw std::logic_error("unhandled optimization algorithm");
}

} // namespace multobjopt
