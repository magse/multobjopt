#ifndef MULTOBJOPT_TYPES_HPP
#define MULTOBJOPT_TYPES_HPP

#include <functional>
#include <span>
#include <string>

/**
 * @file types.hpp
 * @brief Fundamental scalar, callback, enumeration, and definition types.
 *
 * This header contains the vocabulary used to describe a multobjopt problem.
 * It is self-contained and may be included without the umbrella header.
 */

/**
 * @brief Tools for bounded, constrained, multi-objective optimisation.
 *
 * All public multobjopt declarations live in this namespace. The library uses
 * user-selected units and performs no implicit unit conversion.
 */
namespace multobjopt {

/**
 * @brief Scalar type used for parameters, callback values, and algorithm settings.
 *
 * The alias is currently double precision. Values retain the physical units
 * chosen by the caller unless an option explicitly describes a normalized or
 * dimensionless quantity.
 */
using scalar = double;

/**
 * @brief Non-owning, read-only view of a contiguous sequence of scalar values.
 *
 * A view passed to a callback is valid only for the duration of that callback.
 * A callback must copy any values it needs after returning and must not retain
 * the span, its iterators, or pointers into its storage.
 */
using scalar_view = std::span<const scalar>;

/**
 * @brief User-supplied scalar objective or restriction callback.
 *
 * The argument contains bounded and quantised parameter values in parameter
 * insertion order. Its values use the units chosen for the corresponding
 * parameter definitions. The callback is expected to be deterministic when
 * deterministic optimisation is required and should return a finite value for
 * a numerically valid design.
 *
 * The input view and its storage are owned by the library and remain valid only
 * until the callback returns. Exceptions thrown by the callback are not caught;
 * they propagate from evaluate_design() or optimize().
 *
 * @see objective_definition
 * @see restriction_definition
 */
using scalar_function = std::function<scalar(scalar_view)>;

/**
 * @brief User-supplied overall design validation callback.
 *
 * The first view contains raw objective values and the second contains raw
 * restriction values, each in definition insertion order. Every objective and
 * restriction has already been evaluated before this callback is invoked.
 * Returning false rejects the design independently of scalar restrictions.
 *
 * Both spans and their elements are temporary, read-only views valid only for
 * the duration of the call. Copy any data that must outlive the callback.
 * Exceptions are allowed to propagate from evaluate_design() or optimize().
 */
using validation_function = std::function<bool(scalar_view, scalar_view)>;

/** @brief Direction in which an objective is improved. */
enum class objective_sense {
    minimize, ///< Smaller raw values are preferred.
    maximize  ///< Larger raw values are preferred.
};

/**
 * @brief Optimisation algorithm requested by the caller or selected automatically.
 *
 * All algorithms use optimizer_options and obey the same evaluation budget and
 * feasibility rules, although their method-specific settings differ.
 */
enum class optimization_algorithm {
    automatic,           ///< Select a method from the validated problem structure.
    simulated_annealing, ///< Temperature-controlled stochastic local search.
    genetic_algorithm,   ///< Population search using Pareto rank and crowding.
    box_method,          ///< Feasible-complex search using reflection and contraction.
    gradient_descent     ///< Projected descent using numerical finite differences.
};

/** @brief Reason an optimisation run stopped. */
enum class termination_reason {
    evaluation_limit,   ///< The complete-design evaluation budget was exhausted.
    iteration_limit,    ///< The method's outer-iteration budget was exhausted.
    stalled,            ///< No material improvement occurred within the stall window.
    converged,          ///< A method-specific convergence condition was satisfied.
    no_feasible_design, ///< The run ended without discovering an acceptable design.
    numerical_failure   ///< A finite search direction or numerical step was unavailable.
};

/**
 * @brief Definition of one bounded design parameter.
 *
 * Bounds and resolution use the caller's units for this parameter. A resolution
 * of zero describes a continuous parameter. A positive resolution admits grid
 * points of the form `lower_bound + k * resolution`, for non-negative integral
 * `k`, without exceeding `upper_bound`. An upper bound that is not aligned with
 * this grid remains a bound and is not added as an extra grid point. Equal bounds
 * describe a fixed continuous parameter.
 */
struct parameter_definition {
    std::string name;        ///< Non-empty, user-facing name unique among parameters.
    scalar lower_bound{0.0}; ///< Finite inclusive lower bound in parameter units.
    scalar upper_bound{1.0}; ///< Finite inclusive continuous upper bound in parameter units.
    scalar resolution{0.0};  ///< Positive grid spacing, or zero for a continuous value.
};

/**
 * @brief Definition of one user-supplied scalar objective.
 *
 * Raw callback values are retained in evaluated_design. The sense controls
 * Pareto comparisons; the positive weight controls selection of a single
 * compromise by the weighted scalar score. For differently scaled or
 * differently dimensioned objectives, choose weights that also provide the
 * desired numerical scaling.
 */
struct objective_definition {
    std::string name; ///< Non-empty, user-facing name unique among objectives.
    objective_sense sense{objective_sense::minimize}; ///< Direction considered better.
    scalar_function function; ///< Callback invoked once per complete design evaluation.
    scalar weight{1.0};       ///< Finite positive coefficient used for compromise ranking.
};

/**
 * @brief Definition of one user-supplied scalar restriction.
 *
 * A raw callback value greater than or equal to zero is feasible. A negative
 * value is a violation. Its contribution to total_violation is normalized by
 * `scale`, so scale should express a characteristic magnitude in the same units
 * as the callback result. Restriction scale affects violation ranking, not the
 * zero boundary itself.
 */
struct restriction_definition {
    std::string name;         ///< Non-empty, user-facing name unique among restrictions.
    scalar_function function; ///< Callback invoked once per complete design evaluation.
    scalar scale{1.0};        ///< Finite positive divisor used to normalize violation magnitude.
};

} // namespace multobjopt

#endif
