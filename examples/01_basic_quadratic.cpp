#include <multobjopt/multobjopt.hpp>

#include <cstddef>
#include <iostream>

/**
 * @file 01_basic_quadratic.cpp
 * @brief Minimal smooth, unconstrained optimization example.
 *
 * The example shows how parameter declaration order maps to callback spans and
 * result vectors, how an objective sense and weight define the scalar search,
 * and why numerical gradient descent is suitable for a differentiable bowl.
 */

namespace {

// These indices mirror the declaration order below. Naming them keeps callback
// and result access readable if the problem grows beyond this minimal example.
constexpr std::size_t x_parameter_index = 0;
constexpr std::size_t y_parameter_index = 1;
constexpr std::size_t squared_error_objective_index = 0;

} // namespace

int main() {
    using namespace multobjopt;

    problem problem_definition;
    // add_parameter() establishes the exact x-then-y order seen by every
    // scalar_view callback and later retained in best_design.parameters.
    problem_definition.add_parameter("x", -5.0, 5.0)
        .add_parameter("y", -5.0, 5.0)
        .add_objective("squared_error", objective_sense::minimize, [](scalar_view values) {
            // values is a temporary read-only span valid only for this call;
            // copy anything that would need to outlive the callback.
            const scalar dx = values[x_parameter_index] - 1.5;
            const scalar dy = values[y_parameter_index] + 0.5;
            return dx * dx + dy * dy;
        });
    // The explicit minimize sense prefers smaller raw errors. The omitted
    // weight defaults to 1.0; weights matter when selecting one compromise
    // from multiple objectives. This problem has no scalar restrictions. If it
    // did, each callback would return a >= 0 feasible margin.

    optimizer_options options;
    // Numerical gradient descent fits this continuous, smooth, unconstrained
    // objective. The initial guess follows the same x-then-y parameter order.
    options.algorithm = optimization_algorithm::gradient_descent;
    options.initial_guess = {-3.0, 4.0};
    options.max_evaluations = 2'000;

    const auto result = optimize(problem_definition, options);
    // best_design is the feasibility-first scalar compromise. Its parameter
    // and raw-objective vectors preserve declaration order; algorithm_used is
    // the method that actually ran and reason explains why it stopped.
    const auto& design = result.best_design;

    std::cout << "algorithm: " << to_string(result.algorithm_used) << '\n'
              << "x: " << design.parameters[x_parameter_index] << '\n'
              << "y: " << design.parameters[y_parameter_index] << '\n'
              << "objective: " << design.objectives[squared_error_objective_index] << '\n';
}
