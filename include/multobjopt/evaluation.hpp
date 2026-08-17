#ifndef MULTOBJOPT_EVALUATION_HPP
#define MULTOBJOPT_EVALUATION_HPP

#include <multobjopt/problem.hpp>
#include <multobjopt/result.hpp>

#include <vector>

/**
 * @file evaluation.hpp
 * @brief Design normalization, model evaluation, and Pareto comparison helpers.
 *
 * These operations expose the same representation and feasibility conventions
 * used internally by all optimisers.
 */

namespace multobjopt {

/**
 * @brief Clamp and quantise a design according to its parameter definitions.
 *
 * Each input is first clamped to its inclusive bounds. A parameter with positive
 * resolution is then snapped to the nearest grid point anchored at lower_bound;
 * a non-aligned upper bound is not introduced as an extra point. The returned
 * vector owns its values and preserves parameter insertion order and units.
 *
 * @param problem_definition Problem providing bounds and grid resolutions.
 * @param parameters Candidate values in parameter insertion order.
 * @return Owned, bounded, and quantised parameter vector.
 *
 * @throws std::invalid_argument If the problem is invalid, the vector size does
 *         not match the number of parameters, or an input value is non-finite.
 */
[[nodiscard]] std::vector<scalar> normalize_design(const problem& problem_definition,
                                                   scalar_view parameters);

/**
 * @brief Normalize and evaluate one design without running an optimiser.
 *
 * Objective callbacks run first in insertion order, followed by restriction
 * callbacks in insertion order, followed by the optional overall validator.
 * Every callback sees values owned by the current evaluation; any scalar_view
 * supplied to a callback is valid only until that callback returns.
 *
 * Raw callback values are retained in the result. Objective senses and weights
 * form scalarized_objective. A restriction is accepted when its raw value is at
 * least `-constraint_tolerance`; negative excess is divided by that
 * restriction's scale and accumulated in total_violation. Feasibility also
 * requires finite results and acceptance by the overall validator.
 *
 * @param problem_definition Problem whose callbacks and definitions are used.
 * @param parameters Candidate values in parameter insertion order and units.
 * @param constraint_tolerance Finite, non-negative raw restriction undershoot.
 * @return An owning record of normalized parameters, raw callback values, and
 *         derived validity, feasibility, violation, and scalar score.
 *
 * @throws std::invalid_argument If the problem or tolerance is invalid, the
 *         vector size is wrong, or a parameter value is non-finite.
 * @throws Any exception emitted by an objective, restriction, or validation
 *         callback; callback exceptions are deliberately not translated.
 */
[[nodiscard]] evaluated_design evaluate_design(const problem& problem_definition,
                                               scalar_view parameters,
                                               scalar constraint_tolerance = 0.0);

/**
 * @brief Test whether @p lhs Pareto-dominates @p rhs for a problem.
 *
 * A feasible design always dominates an infeasible design. Between infeasible
 * designs, a smaller total_violation must improve by more than @p tolerance.
 * Between feasible designs, objective senses are honored: lhs must be exactly
 * no worse in every component and better by more than @p tolerance in at least
 * one. Objective weights do not affect Pareto dominance.
 *
 * This function compares existing records and invokes no user callback.
 *
 * @param problem_definition Problem supplying objective senses and count.
 * @param lhs Candidate proposed as the dominating design.
 * @param rhs Candidate proposed as the dominated design.
 * @param tolerance Finite non-negative threshold for a strict improvement.
 * @return true exactly when lhs dominates rhs under the rules above.
 *
 * @throws std::invalid_argument If tolerance is invalid, or when both designs
 *         are feasible and an objective vector size does not match the problem.
 */
[[nodiscard]] bool dominates(const problem& problem_definition, const evaluated_design& lhs,
                             const evaluated_design& rhs, scalar tolerance = 1.0e-12);

} // namespace multobjopt

#endif
