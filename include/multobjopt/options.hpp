#ifndef MULTOBJOPT_OPTIONS_HPP
#define MULTOBJOPT_OPTIONS_HPP

#include <multobjopt/types.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>

/**
 * @file options.hpp
 * @brief Common and method-specific optimiser controls.
 *
 * optimizer_options provides one stable configuration object for every solver.
 * This header is self-contained and may be included directly.
 */

namespace multobjopt {

/**
 * @brief Common options structure controlling every optimiser.
 *
 * Options irrelevant to the selected method do not affect its search, but the
 * complete structure is validated so configuration errors are reported early.
 * Fields described as a scale relative to a parameter range are dimensionless;
 * raw tolerances and merit values otherwise follow the caller's objective or
 * restriction value conventions.
 */
struct optimizer_options {
    optimization_algorithm algorithm{optimization_algorithm::automatic}; ///< Method to run.

    /** Maximum number of complete design evaluations, always positive. */
    std::size_t max_evaluations{10'000};

    /** Maximum number of method-specific outer iterations, always positive. */
    std::size_t max_iterations{1'000};

    /** Consecutive non-improving iterations allowed before a stalled result. */
    std::size_t stall_iterations{150};

    /** Seed used to initialize deterministic pseudo-random searches. */
    std::uint64_t random_seed{5'489};

    /**
     * Non-negative raw undershoot allowed for every scalar restriction.
     *
     * A restriction is accepted when its value is at least
     * `-constraint_tolerance`; choose compatible callback units when different
     * restrictions share this single tolerance.
     */
    scalar constraint_tolerance{0.0};

    /** Finite positive merit multiplier applied to infeasible-design violations. */
    scalar penalty_factor{1.0e6};

    /** Minimum merit improvement that resets method-specific stall accounting. */
    scalar improvement_tolerance{1.0e-10};

    /**
     * Optional starting design in parameter units and insertion order.
     *
     * An empty vector selects the bounded midpoint. A supplied vector is
     * clamped and quantised before use and must contain one finite value per
     * parameter.
     */
    std::vector<scalar> initial_guess;

    // Simulated annealing options.

    /** Finite positive initial acceptance temperature in merit-value scale. */
    scalar initial_temperature{1.0};

    /** Positive terminal temperature, strictly below initial_temperature. */
    scalar final_temperature{1.0e-6};

    /** Geometric temperature multiplier strictly between zero and one. */
    scalar cooling_rate{0.95};

    /** Positive initial proposal size relative to each parameter range. */
    scalar annealing_step_scale{0.20};

    /** Moves attempted at each temperature; zero selects a dimension-based default. */
    std::size_t moves_per_temperature{0};

    // Genetic algorithm options.

    /** Population target; must contain at least two individuals. */
    std::size_t population_size{64};

    /** Positive number of leading individuals retained, below population_size. */
    std::size_t elite_count{2};

    /** Positive number of candidates drawn for each parent tournament. */
    std::size_t tournament_size{3};

    /** Per-parameter blend probability in the inclusive range [0, 1]. */
    scalar crossover_rate{0.85};

    /** Per-parameter mutation probability in the inclusive range [0, 1]. */
    scalar mutation_rate{0.12};

    /** Positive mutation standard-deviation scale relative to parameter range. */
    scalar mutation_scale{0.10};

    // Box complex-method options.

    /** Number of Box-complex points; zero selects a dimension-based default. */
    std::size_t complex_size{0};

    /** Positive reflection distance multiplier through the complex centroid. */
    scalar reflection_factor{1.3};

    /** Failed-reflection contraction multiplier strictly between zero and one. */
    scalar contraction_factor{0.5};

    // Numerical projected-gradient options.

    /** Positive finite-difference displacement relative to parameter range. */
    scalar finite_difference_step{1.0e-5};

    /** Positive initial normalized line-search displacement. */
    scalar initial_step_scale{0.20};

    /** Backtracking line-search multiplier strictly between zero and one. */
    scalar line_search_decay{0.5};

    /** Non-negative convergence threshold for the normalized gradient norm. */
    scalar gradient_tolerance{1.0e-7};

    /** Positive smallest line-search scale, below initial_step_scale. */
    scalar minimum_step_scale{1.0e-10};

    /**
     * Retain every completed design evaluation in the optimization result.
     *
     * This is disabled by default so ordinary runs do not pay the potentially
     * significant memory cost of keeping the complete search trajectory. When
     * enabled, optimization_result::evaluation_history contains one owning
     * evaluated_design for every counted evaluation, in evaluation order.
     */
    bool record_evaluation_history{false};

    /**
     * @brief Validate every common and method-specific option.
     *
     * The full structure is checked even when the selected algorithm does not
     * consume some fields. This function does not require or inspect a problem.
     *
     * @throws std::invalid_argument If an option is non-finite, outside its
     *         documented range, or inconsistent with a related option.
     */
    void validate() const;
};

} // namespace multobjopt

#endif
