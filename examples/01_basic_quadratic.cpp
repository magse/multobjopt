#include <multobjopt/multobjopt.hpp>

#include <iostream>

int main() {
    using namespace multobjopt;

    problem problem_definition;
    problem_definition.add_parameter("x", -5.0, 5.0)
        .add_parameter("y", -5.0, 5.0)
        .add_objective("squared_error", objective_sense::minimize, [](scalar_view values) {
            const scalar dx = values[0] - 1.5;
            const scalar dy = values[1] + 0.5;
            return dx * dx + dy * dy;
        });

    optimizer_options options;
    options.algorithm = optimization_algorithm::gradient_descent;
    options.initial_guess = {-3.0, 4.0};
    options.max_evaluations = 2'000;

    const auto result = optimize(problem_definition, options);
    const auto& design = result.best_design;

    std::cout << "algorithm: " << to_string(result.algorithm_used) << '\n'
              << "x: " << design.parameters[0] << '\n'
              << "y: " << design.parameters[1] << '\n'
              << "objective: " << design.objectives[0] << '\n';
}
