#ifndef MULTOBJOPT_DETAIL_OPTIMIZER_DETAIL_HPP
#define MULTOBJOPT_DETAIL_OPTIMIZER_DETAIL_HPP

#include <multobjopt/multobjopt.hpp>

#include <cstddef>
#include <random>
#include <vector>

/**
 * @file optimizer_detail.hpp
 * @brief Shared implementation facilities used by the optimization algorithms.
 *
 * Nothing in this header is part of the installed public API. It centralizes
 * projection, evaluation accounting, feasibility-first ordering, Pareto
 * archiving, deterministic point generation, and common result construction.
 */

namespace multobjopt::detail {

/** @brief Return whether a scalar is neither infinite nor NaN. */
[[nodiscard]] bool finite(scalar value) noexcept;

/**
 * @brief Enforce a configuration precondition.
 * @throws std::invalid_argument when condition is false.
 */
void require(bool condition, const char* message);

/** @brief Convert an objective value to the internal minimization direction. */
[[nodiscard]] scalar canonical_objective(const objective_definition& objective,
                                         scalar value) noexcept;

/** @brief Return a finite sortable objective, mapping invalid values to infinity. */
[[nodiscard]] scalar sortable_objective(const objective_definition& objective,
                                        scalar value) noexcept;

/** @brief Project one value to its inclusive bounds and resolution lattice. */
[[nodiscard]] scalar normalize_parameter(const parameter_definition& definition, scalar value);

/**
 * @brief Normalize a design after the complete problem has already been validated.
 * @throws std::invalid_argument when the design vector has the wrong size.
 */
[[nodiscard]] std::vector<scalar> normalize_design_unchecked(const problem& problem_definition,
                                                             scalar_view parameters);

/** @brief Evaluate callbacks without revalidating an already validated problem. */
[[nodiscard]] evaluated_design evaluate_unchecked(const problem& problem_definition,
                                                  scalar_view parameters,
                                                  scalar constraint_tolerance);

/** @brief Compare two scalar ranges lexicographically. */
[[nodiscard]] bool lexicographically_less(scalar_view lhs, scalar_view rhs) noexcept;

/** @brief Apply the deterministic feasibility-first scalar ordering. */
[[nodiscard]] bool rank_before(const evaluated_design& lhs, const evaluated_design& rhs) noexcept;

/** @brief Test whether progress is larger than the configured stall tolerance. */
[[nodiscard]] bool significant_improvement(const evaluated_design& current,
                                           const evaluated_design& previous,
                                           scalar tolerance) noexcept;

/** @brief Test equality of raw objective vectors within a tolerance. */
[[nodiscard]] bool equal_objectives(const evaluated_design& lhs, const evaluated_design& rhs,
                                    scalar tolerance) noexcept;

/**
 * @brief Budgeted evaluator shared by every optimizer.
 *
 * Each successful call to evaluate() consumes exactly one complete design
 * evaluation, updates the feasibility-first best design, and updates the
 * feasible nondominated archive.
 */
class evaluator {
  public:
    /** @brief Bind an evaluator to one validated problem and options object. */
    evaluator(const problem& problem_definition, const optimizer_options& options);

    /** @brief Return whether another complete callback evaluation is permitted. */
    [[nodiscard]] bool can_evaluate() const noexcept;

    /**
     * @brief Normalize and evaluate one design.
     * @throws std::logic_error if the evaluation budget is already exhausted.
     */
    [[nodiscard]] evaluated_design evaluate(scalar_view parameters);

    /** @brief Number of complete designs evaluated so far. */
    [[nodiscard]] std::size_t evaluations() const noexcept;

    /** @brief Current feasibility-first scalar best design. */
    [[nodiscard]] const evaluated_design& best() const noexcept;

    /** @brief Monotonic revision counter for Pareto archive changes. */
    [[nodiscard]] std::size_t archive_revision() const noexcept;

    /** @brief Return the Pareto archive in deterministic objective order. */
    [[nodiscard]] std::vector<evaluated_design> archive() const;

  private:
    /** @brief Insert a feasible design while preserving nondominance and uniqueness. */
    void update_archive(const evaluated_design& candidate);

    const problem& problem_;
    const optimizer_options& options_;
    std::size_t evaluations_{0};
    bool has_best_{false};
    evaluated_design best_;
    std::vector<evaluated_design> archive_;
    std::size_t archive_revision_{0};
};

/** @brief Return the normalized midpoint of every parameter interval. */
[[nodiscard]] std::vector<scalar> midpoint(const problem& problem_definition);

/** @brief Return the configured initial guess or the normalized midpoint. */
[[nodiscard]] std::vector<scalar> starting_point(const problem& problem_definition,
                                                 const optimizer_options& options);

/** @brief Draw one bounded point, sampling resolution grids without endpoint bias. */
[[nodiscard]] std::vector<scalar> random_point(const problem& problem_definition,
                                               std::mt19937_64& generator);

/** @brief Count parameters whose lower and upper bounds differ. */
[[nodiscard]] std::size_t free_parameter_count(const problem& problem_definition);

/** @brief Assemble a public result from shared evaluator state. */
[[nodiscard]] optimization_result finish_result(optimization_algorithm algorithm,
                                                const evaluator& problem_evaluator,
                                                std::size_t iterations, termination_reason reason);

/** @brief Run simulated annealing with the shared problem and options contract. */
[[nodiscard]] optimization_result run_simulated_annealing(const problem& problem_definition,
                                                          const optimizer_options& options);

/** @brief Run the Pareto-aware genetic algorithm. */
[[nodiscard]] optimization_result run_genetic_algorithm(const problem& problem_definition,
                                                        const optimizer_options& options);

/** @brief Run the feasible-complex Box method. */
[[nodiscard]] optimization_result run_box_method(const problem& problem_definition,
                                                 const optimizer_options& options);

/** @brief Run projected numerical-gradient descent. */
[[nodiscard]] optimization_result run_gradient_descent(const problem& problem_definition,
                                                       const optimizer_options& options);

} // namespace multobjopt::detail

#endif
