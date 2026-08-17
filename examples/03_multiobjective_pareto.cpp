#include <multobjopt/multobjopt.hpp>

#include <algorithm>
#include <cstddef>
#include <iostream>

int main() {
    using namespace multobjopt;

    // Moving x toward either endpoint improves one objective and worsens the
    // other.
    problem problem_definition;
    problem_definition.add_parameter("x", 0.0, 1.0)
        .add_objective("distance_from_zero", objective_sense::minimize,
                       [](scalar_view values) { return values[0] * values[0]; })
        .add_objective("distance_from_one", objective_sense::minimize, [](scalar_view values) {
            const scalar distance = values[0] - 1.0;
            return distance * distance;
        });

    optimizer_options options;
    options.algorithm = optimization_algorithm::genetic_algorithm;
    options.population_size = 48;
    options.max_evaluations = 2'000;
    options.stall_iterations = 80;

    const auto result = optimize(problem_definition, options);
    std::cout << "pareto designs: " << result.pareto_front.size() << '\n';

    const std::size_t shown = std::min<std::size_t>(7, result.pareto_front.size());
    for (std::size_t sample = 0; sample < shown; ++sample) {
        const std::size_t index =
            shown == 1 ? 0 : sample * (result.pareto_front.size() - 1) / (shown - 1);
        const auto& design = result.pareto_front[index];
        std::cout << "x=" << design.parameters[0] << " objectives=(" << design.objectives[0] << ", "
                  << design.objectives[1] << ")\n";
    }
}
