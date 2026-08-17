#include <multobjopt/multobjopt.hpp>

#include <iostream>

int main() {
    using namespace multobjopt;

    // Find the least-material rectangle whose area is at least one.
    problem problem_definition;
    problem_definition.add_parameter("width", 0.2, 3.0)
        .add_parameter("height", 0.2, 3.0)
        .add_objective(
            "material", objective_sense::minimize,
            [](scalar_view values) { return values[0] * values[0] + values[1] * values[1]; })
        .add_restriction("minimum_area",
                         [](scalar_view values) { return values[0] * values[1] - 1.0; });

    optimizer_options options;
    options.algorithm = optimization_algorithm::box_method;
    options.initial_guess = {1.2, 1.2};
    options.max_evaluations = 4'000;
    options.stall_iterations = 250;

    const auto result = optimize(problem_definition, options);
    const auto& design = result.best_design;

    std::cout << "algorithm: " << to_string(result.algorithm_used) << '\n'
              << "width: " << design.parameters[0] << '\n'
              << "height: " << design.parameters[1] << '\n'
              << "area margin: " << design.restrictions[0] << '\n'
              << "feasible: " << std::boolalpha << design.feasible << '\n';
}
