#ifndef MULTOBJOPT_PROBLEM_HPP
#define MULTOBJOPT_PROBLEM_HPP

#include <multobjopt/types.hpp>

#include <string>
#include <vector>

/**
 * @file problem.hpp
 * @brief Builder and validated storage for user-defined optimisation problems.
 *
 * The problem class owns parameter, objective, and restriction definitions and
 * preserves their insertion order. It is self-contained and can be included
 * directly by applications that do not need solver declarations.
 */

namespace multobjopt {

/**
 * @brief Complete user-defined optimisation problem.
 *
 * The fluent add functions make small problems concise, while the accessors
 * expose definitions to applications that construct problems programmatically.
 * Add operations store the supplied definitions without eagerly validating the
 * whole problem. Call validate() at a configuration boundary for early errors;
 * normalize_design(), evaluate_design(), select_algorithm(), and optimize() also
 * validate before use.
 *
 * Definition and callback order is significant. Parameter callbacks receive
 * parameter values in parameter insertion order, and the overall validation
 * callback receives objective and restriction results in their respective
 * insertion orders.
 */
class problem {
  public:
    /**
     * @brief Append a bounded parameter definition.
     * @param parameter Definition to store by value.
     * @return This problem, enabling fluent calls.
     *
     * Validation is deferred until validate() or an operation that validates.
     */
    problem& add_parameter(parameter_definition parameter);

    /**
     * @brief Construct and append a bounded parameter definition.
     * @param name Non-empty name unique among parameters.
     * @param lower_bound Inclusive lower bound in caller-selected units.
     * @param upper_bound Inclusive upper bound in the same units.
     * @param resolution Positive grid spacing, or zero for continuous values.
     * @return This problem, enabling fluent calls.
     *
     * Validation is deferred until validate() or an operation that validates.
     */
    problem& add_parameter(std::string name, scalar lower_bound, scalar upper_bound,
                           scalar resolution = 0.0);

    /**
     * @brief Append an objective definition.
     * @param objective Definition and callable to store by value.
     * @return This problem, enabling fluent calls.
     *
     * Validation is deferred until validate() or an operation that validates.
     */
    problem& add_objective(objective_definition objective);

    /**
     * @brief Construct and append a scalar objective definition.
     * @param name Non-empty name unique among objectives.
     * @param sense Direction in which the raw callback value is improved.
     * @param function Callable receiving the current parameter vector.
     * @param weight Finite positive scalar-compromise coefficient.
     * @return This problem, enabling fluent calls.
     *
     * The callable is invoked during evaluation, not by this function.
     */
    problem& add_objective(std::string name, objective_sense sense, scalar_function function,
                           scalar weight = 1.0);

    /**
     * @brief Append a restriction definition.
     * @param restriction Definition and callable to store by value.
     * @return This problem, enabling fluent calls.
     *
     * A restriction is feasible at raw values greater than or equal to zero.
     * Validation is deferred until validate() or an operation that validates.
     */
    problem& add_restriction(restriction_definition restriction);

    /**
     * @brief Construct and append a scalar restriction definition.
     * @param name Non-empty name unique among restrictions.
     * @param function Callable whose non-negative result is feasible.
     * @param scale Finite positive divisor for normalized violation magnitude.
     * @return This problem, enabling fluent calls.
     *
     * The callable is invoked during evaluation, not by this function.
     */
    problem& add_restriction(std::string name, scalar_function function, scalar scale = 1.0);

    /**
     * @brief Set, replace, or clear the optional overall validation callback.
     * @param validation Callable receiving separate objective and restriction
     *        spans. Passing an empty std::function clears the current validator.
     * @return This problem, enabling fluent calls.
     *
     * This function only stores the callable; it does not evaluate the problem.
     * During each later complete design evaluation, all objectives run first in
     * objective insertion order, all restrictions run next in restriction
     * insertion order, and this validator runs last. Its first span contains
     * every raw objective result in objective order, and its second span contains
     * every raw restriction result in restriction order. Objective senses and
     * weights, restriction scales, and constraint tolerance are not applied to
     * these values.
     *
     * The validator is invoked even when a preceding scalar callback produced a
     * nonfinite raw value. Nonfinite scalar results make the design numerically
     * invalid regardless of the returned Boolean. For otherwise finite results,
     * returning false leaves the design numerically valid but rejects its
     * feasibility. Since a Boolean veto supplies no distance to acceptance, it
     * adds one unit to total violation. Returning true cannot override a negative
     * restriction margin or otherwise turn an invalid or infeasible design into
     * a feasible one.
     *
     * Both spans are temporary, read-only views valid only for that invocation;
     * the callback must not retain the spans, their iterators, or pointers to
     * their elements. Exceptions thrown during later invocation propagate from
     * evaluate_design() or optimize(). Prefer deterministic, side-effect-free
     * validation so repeated evaluations and fixed random seeds remain
     * reproducible.
     */
    problem& set_validation(validation_function validation);

    /**
     * @brief Access parameter definitions in insertion order.
     * @return Const reference owned by this problem.
     *
     * The reference and its elements remain valid until a non-const operation
     * modifies the parameter collection or the problem is destroyed.
     */
    [[nodiscard]] const std::vector<parameter_definition>& parameters() const noexcept;

    /**
     * @brief Access objective definitions in insertion order.
     * @return Const reference owned by this problem.
     *
     * The reference and its elements remain valid until a non-const operation
     * modifies the objective collection or the problem is destroyed.
     */
    [[nodiscard]] const std::vector<objective_definition>& objectives() const noexcept;

    /**
     * @brief Access restriction definitions in insertion order.
     * @return Const reference owned by this problem.
     *
     * The reference and its elements remain valid until a non-const operation
     * modifies the restriction collection or the problem is destroyed.
     */
    [[nodiscard]] const std::vector<restriction_definition>& restrictions() const noexcept;

    /**
     * @brief Access the optional overall validation callback.
     * @return Const reference to the stored callable; it is empty when unset.
     *
     * The reference remains valid until set_validation() is called or the
     * problem is destroyed.
     */
    [[nodiscard]] const validation_function& validation() const noexcept;

    /**
     * @brief Check the complete problem definition for consistency.
     *
     * A valid problem has one to 25 parameters, at least one objective, finite
     * ordered bounds, valid resolutions, unique non-empty names within each
     * definition category, non-empty scalar callbacks, and finite positive
     * objective weights and restriction scales.
     *
     * This function validates configuration only and does not invoke callbacks.
     *
     * @throws std::invalid_argument If any definition is inconsistent.
     */
    void validate() const;

  private:
    std::vector<parameter_definition> parameters_;
    std::vector<objective_definition> objectives_;
    std::vector<restriction_definition> restrictions_;
    validation_function validation_;
};

} // namespace multobjopt

#endif
