#include "detail/optimizer_detail.hpp"

/**
 * @file optimizer_options_validate.cpp
 * @brief Validation of the common optimizer options structure.
 *
 * Every field is checked regardless of the selected algorithm. This keeps the
 * options object portable between methods and reports invalid configuration at
 * the public optimization boundary instead of partway through a search.
 */

namespace multobjopt {

/**
 * @brief Validate all common and algorithm-specific option invariants.
 * @throws std::invalid_argument when a count, scale, probability, temperature,
 *         tolerance, or coupled bound is outside its documented domain.
 *
 * Coupled settings are checked together: the final annealing temperature must
 * be below the initial temperature, the elite set must be smaller than the
 * population, and the minimum gradient step must be below the initial step.
 */
void optimizer_options::validate() const {
    detail::require(max_evaluations > 0, "max_evaluations must be positive");
    detail::require(max_iterations > 0, "max_iterations must be positive");
    detail::require(stall_iterations > 0, "stall_iterations must be positive");
    detail::require(detail::finite(constraint_tolerance) && constraint_tolerance >= 0.0,
                    "constraint_tolerance must be finite and non-negative");
    detail::require(detail::finite(penalty_factor) && penalty_factor > 0.0,
                    "penalty_factor must be finite and positive");
    detail::require(detail::finite(improvement_tolerance) && improvement_tolerance >= 0.0,
                    "improvement_tolerance must be finite and non-negative");

    detail::require(detail::finite(initial_temperature) && initial_temperature > 0.0,
                    "initial_temperature must be finite and positive");
    detail::require(detail::finite(final_temperature) && final_temperature > 0.0 &&
                        final_temperature < initial_temperature,
                    "final_temperature must be positive and below initial_temperature");
    detail::require(detail::finite(cooling_rate) && cooling_rate > 0.0 && cooling_rate < 1.0,
                    "cooling_rate must be between zero and one");
    detail::require(detail::finite(annealing_step_scale) && annealing_step_scale > 0.0,
                    "annealing_step_scale must be finite and positive");

    detail::require(population_size >= 2, "population_size must be at least two");
    detail::require(elite_count > 0 && elite_count < population_size,
                    "elite_count must be positive and smaller than population_size");
    detail::require(tournament_size > 0, "tournament_size must be positive");
    detail::require(detail::finite(crossover_rate) && crossover_rate >= 0.0 &&
                        crossover_rate <= 1.0,
                    "crossover_rate must be between zero and one");
    detail::require(detail::finite(mutation_rate) && mutation_rate >= 0.0 && mutation_rate <= 1.0,
                    "mutation_rate must be between zero and one");
    detail::require(detail::finite(mutation_scale) && mutation_scale > 0.0,
                    "mutation_scale must be finite and positive");

    detail::require(complex_size == 0 || complex_size >= 2,
                    "complex_size must be zero or at least two");
    detail::require(detail::finite(reflection_factor) && reflection_factor > 0.0,
                    "reflection_factor must be finite and positive");
    detail::require(detail::finite(contraction_factor) && contraction_factor > 0.0 &&
                        contraction_factor < 1.0,
                    "contraction_factor must be between zero and one");

    detail::require(detail::finite(finite_difference_step) && finite_difference_step > 0.0,
                    "finite_difference_step must be finite and positive");
    detail::require(detail::finite(initial_step_scale) && initial_step_scale > 0.0,
                    "initial_step_scale must be finite and positive");
    detail::require(detail::finite(line_search_decay) && line_search_decay > 0.0 &&
                        line_search_decay < 1.0,
                    "line_search_decay must be between zero and one");
    detail::require(detail::finite(gradient_tolerance) && gradient_tolerance >= 0.0,
                    "gradient_tolerance must be finite and non-negative");
    detail::require(detail::finite(minimum_step_scale) && minimum_step_scale > 0.0 &&
                        minimum_step_scale < initial_step_scale,
                    "minimum_step_scale must be positive and below initial_step_scale");
}

} // namespace multobjopt
