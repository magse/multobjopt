#include "detail/optimizer_detail.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

/**
 * @file evaluate_unchecked.cpp
 * @brief Internal complete-design evaluation for an already validated problem.
 *
 * Objective callbacks run first, restriction callbacks run second, and the
 * optional overall validator receives both complete raw value vectors last.
 */

namespace multobjopt::detail {

/**
 * @brief Evaluate one complete design after problem validation.
 * @param problem_definition Validated problem whose callbacks are invoked.
 * @param parameters Proposed design in parameter-definition order.
 * @param constraint_tolerance Allowed undershoot of each restriction.
 * @return Raw callback values plus normalized feasibility and ranking fields.
 *
 * Each restriction contributes `max(0, (-value - tolerance) / scale)` to total
 * violation. A rejected Boolean validation has no distance measure, so it
 * contributes one unit. Any nonfinite callback value makes the design invalid
 * and gives it infinite violation.
 */
evaluated_design evaluate_unchecked(const problem& problem_definition, scalar_view parameters,
                                    scalar constraint_tolerance) {
    evaluated_design evaluation;
    evaluation.parameters.reserve(parameters.size());
    for (std::size_t index = 0; index < parameters.size(); ++index) {
        evaluation.parameters.push_back(
            normalize_parameter(problem_definition.parameters()[index], parameters[index]));
    }

    evaluation.objectives.reserve(problem_definition.objectives().size());
    long double scalarized = 0.0L;
    bool callbacks_are_finite = true;
    for (const auto& objective : problem_definition.objectives()) {
        const scalar value = objective.function(evaluation.parameters);
        evaluation.objectives.push_back(value);
        if (!finite(value)) {
            callbacks_are_finite = false;
            continue;
        }
        scalarized += static_cast<long double>(objective.weight) *
                      static_cast<long double>(canonical_objective(objective, value));
    }
    evaluation.scalarized_objective =
        callbacks_are_finite && std::isfinite(scalarized) &&
                std::abs(scalarized) <= static_cast<long double>(std::numeric_limits<scalar>::max())
            ? static_cast<scalar>(scalarized)
            : std::numeric_limits<scalar>::infinity();

    evaluation.restrictions.reserve(problem_definition.restrictions().size());
    scalar violation = 0.0;
    for (const auto& restriction : problem_definition.restrictions()) {
        const scalar value = restriction.function(evaluation.parameters);
        evaluation.restrictions.push_back(value);
        if (!finite(value)) {
            callbacks_are_finite = false;
            violation = std::numeric_limits<scalar>::infinity();
        } else if (finite(violation)) {
            violation += std::max(0.0, (-value - constraint_tolerance) / restriction.scale);
        }
    }

    // A failed numerical model is less useful than any finite infeasible
    // design, even if its explicit restrictions happened to be satisfied.
    evaluation.valid =
        callbacks_are_finite && finite(evaluation.scalarized_objective) && finite(violation);
    if (!evaluation.valid) {
        violation = std::numeric_limits<scalar>::infinity();
    }

    const bool accepted_by_validator =
        !problem_definition.validation() ||
        problem_definition.validation()(evaluation.objectives, evaluation.restrictions);
    if (!accepted_by_validator && finite(violation)) {
        // A Boolean validator exposes no distance to feasibility. One unit keeps
        // rejected designs behind accepted designs while allowing deterministic
        // ordering through their ordinary restriction violations.
        violation += 1.0;
    }

    evaluation.total_violation = violation;
    evaluation.feasible = evaluation.valid && accepted_by_validator && violation == 0.0;
    return evaluation;
}

} // namespace multobjopt::detail
