#ifndef MULTOBJOPT_RESULT_HPP
#define MULTOBJOPT_RESULT_HPP

#include <multobjopt/types.hpp>

#include <cstddef>
#include <limits>
#include <vector>

/**
 * @file result.hpp
 * @brief Evaluation and complete optimisation result value types.
 *
 * Result objects own all of their vectors; unlike callback scalar_view values,
 * their contents remain valid according to normal C++ object lifetime rules.
 */

namespace multobjopt {

/**
 * @brief Objective, restriction, validity, and feasibility data for one design.
 *
 * The design stores both raw model outputs and derived ranking values. `valid`
 * reports numerical validity, while `feasible` additionally includes scalar
 * restrictions and the optional overall validator. A validator rejection does
 * not by itself make a numerically finite design invalid.
 */
struct evaluated_design {
    /** Bounded and quantised parameter values in definition order and parameter units. */
    std::vector<scalar> parameters;

    /** Raw objective callback values in objective definition order and callback units. */
    std::vector<scalar> objectives;

    /** Raw restriction callback values in restriction definition order and callback units. */
    std::vector<scalar> restrictions;

    /** True when callback outputs, aggregate violation, and scalar score are all finite. */
    bool valid{false};

    /** True when valid, all restrictions meet tolerance, and the validator accepts. */
    bool feasible{false};

    /**
     * Sum of normalized scalar violations, with validator rejection represented
     * by an additional unit violation; infinity denotes numerical invalidity.
     */
    scalar total_violation{std::numeric_limits<scalar>::infinity()};

    /**
     * Weighted signed objective sum used to choose a single compromise.
     *
     * Minimized objectives contribute `weight * value`; maximized objectives
     * contribute `-weight * value`. This score does not replace Pareto
     * dominance. Infinity denotes an invalid or unrepresentable aggregate.
     */
    scalar scalarized_objective{std::numeric_limits<scalar>::infinity()};
};

/**
 * @brief Complete value-semantic result of an optimisation run.
 *
 * A completed run always records the method actually executed and its consumed
 * budgets. If no feasible point was found, reason is no_feasible_design and
 * best_design is the feasibility-first best attempted design. The Pareto front
 * never contains infeasible entries.
 */
struct optimization_result {
    /** Actual method executed; never automatic for a normally completed run. */
    optimization_algorithm algorithm_used{optimization_algorithm::automatic};

    /** Feasibility-first best design, using objective weights for feasible ties. */
    evaluated_design best_design;

    /** Feasible nondominated designs, ordered deterministically by objectives and parameters. */
    std::vector<evaluated_design> pareto_front;

    /** Number of complete design evaluations, never above max_evaluations. */
    std::size_t evaluations{0};

    /** Number of method-specific outer iterations reported by the solver. */
    std::size_t iterations{0};

    /** Stop condition, or no_feasible_design when the feasible archive is empty. */
    termination_reason reason{termination_reason::evaluation_limit};
};

} // namespace multobjopt

#endif
