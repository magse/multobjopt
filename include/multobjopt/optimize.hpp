#ifndef MULTOBJOPT_OPTIMIZE_HPP
#define MULTOBJOPT_OPTIMIZE_HPP

#include <multobjopt/options.hpp>
#include <multobjopt/problem.hpp>
#include <multobjopt/result.hpp>

#include <string_view>

/**
 * @file optimize.hpp
 * @brief Algorithm selection, optimisation entry point, and enum names.
 *
 * Include this header for running a configured problem. It is self-contained;
 * multobjopt.hpp remains the convenient umbrella for the complete API.
 */

namespace multobjopt {

/**
 * @brief Select a suitable algorithm from validated problem structure.
 *
 * The deterministic structural policy is:
 *
 * - multiple objectives or any quantised parameter: genetic_algorithm;
 * - otherwise, any restriction or overall validator: box_method;
 * - otherwise, at most 12 non-fixed parameters: gradient_descent;
 * - otherwise: simulated_annealing.
 *
 * Opaque callbacks are not sampled or inspected for smoothness. Callers that
 * know more about their model may explicitly choose another algorithm.
 *
 * @param problem_definition Problem whose counts and definitions are inspected.
 * @return The concrete algorithm selected; never automatic.
 * @throws std::invalid_argument If the problem definition is invalid.
 */
[[nodiscard]] optimization_algorithm select_algorithm(const problem& problem_definition);

/**
 * @brief Optimise a bounded, constrained, multi-objective problem.
 *
 * Both inputs are validated before any design callback is invoked. If options
 * request automatic selection, select_algorithm() determines the concrete
 * method recorded in optimization_result::algorithm_used. Each counted
 * evaluation is one complete set of objective, restriction, and optional
 * validation calls, regardless of the number of callbacks, and the result never
 * exceeds optimizer_options::max_evaluations. This function performs no file
 * or console output; completed results can be passed explicitly to the writers
 * declared in reporting.hpp.
 *
 * @param problem_definition Problem and callbacks to evaluate. It must remain
 *        alive and must not be mutated for the duration of this synchronous call.
 * @param options Common and method-specific controls. The default options use
 *        automatic selection and deterministic default seed.
 * @return Owning best-design, Pareto-front, accounting, termination data, and
 *         optional evaluation history when its recording option was enabled.
 *
 * @throws std::invalid_argument If the problem, options, or initial guess is
 *         invalid or inconsistent.
 * @throws Any exception emitted by an objective, restriction, or validation
 *         callback; callback exceptions are deliberately not translated.
 *
 * @see write_evaluation_history
 * @see write_pareto_front
 * @see write_summary_json
 */
[[nodiscard]] optimization_result optimize(const problem& problem_definition,
                                           const optimizer_options& options = {});

/**
 * @brief Return the stable snake_case name of an algorithm value.
 * @param algorithm Enumeration value to name.
 * @return View into static storage, or `"unknown"` for an unrecognized value.
 */
[[nodiscard]] std::string_view to_string(optimization_algorithm algorithm) noexcept;

/**
 * @brief Return the stable snake_case name of a termination value.
 * @param reason Enumeration value to name.
 * @return View into static storage, or `"unknown"` for an unrecognized value.
 */
[[nodiscard]] std::string_view to_string(termination_reason reason) noexcept;

} // namespace multobjopt

#endif
