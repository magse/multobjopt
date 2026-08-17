#include <multobjopt/multobjopt.hpp>

#include <iostream>

int main() {
    using namespace multobjopt;

    // Select a quantized gearbox and arm geometry. The shared formulas stand in
    // for a simulation that could instead be captured by each callable.
    const auto payload = [](scalar_view values) {
        const scalar gear_teeth = values[0];
        const scalar motor_current = values[1];
        const scalar arm_length = values[2];
        return 0.018 * gear_teeth * motor_current / arm_length;
    };

    problem problem_definition;
    problem_definition.add_parameter("gear_teeth", 12.0, 60.0, 1.0)
        .add_parameter("motor_current", 2.0, 16.0, 0.5)
        .add_parameter("arm_length", 0.20, 0.80, 0.01)
        .add_objective("moving_mass", objective_sense::minimize,
                       [](scalar_view values) {
                           return 1.0 + 0.018 * values[0] + 0.060 * values[1] + 1.2 * values[2];
                       })
        .add_objective("payload", objective_sense::maximize, payload)
        .add_restriction("thermal_margin",
                         [](scalar_view values) { return 130.0 - 0.55 * values[1] * values[1]; })
        .add_restriction("tip_speed_margin",
                         [](scalar_view values) { return 1.8 - 0.05 * values[0] * values[2]; })
        .add_restriction("structural_payload_margin",
                         [payload](scalar_view values) { return 35.0 - payload(values); })
        .set_validation([](scalar_view objectives, scalar_view restrictions) {
            // Final assembly acceptance can combine every computed result.
            return objectives[0] <= 3.5 && objectives[1] >= 2.0 && restrictions.size() == 3;
        });

    optimizer_options options;
    options.algorithm = optimization_algorithm::automatic;
    options.population_size = 64;
    options.max_evaluations = 5'000;
    options.stall_iterations = 120;

    const auto result = optimize(problem_definition, options);
    const auto& design = result.best_design;

    std::cout << "selected algorithm: " << to_string(result.algorithm_used) << '\n'
              << "gear teeth: " << design.parameters[0] << '\n'
              << "motor current: " << design.parameters[1] << '\n'
              << "arm length: " << design.parameters[2] << '\n'
              << "moving mass: " << design.objectives[0] << '\n'
              << "payload: " << design.objectives[1] << '\n'
              << "pareto designs: " << result.pareto_front.size() << '\n';
}
