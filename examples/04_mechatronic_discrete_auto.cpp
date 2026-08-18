#include <multobjopt/multobjopt.hpp>

#include <cstddef>
#include <iostream>

/**
 * @file 04_mechatronic_discrete_auto.cpp
 * @brief Mixed-resolution, constrained, multiobjective mechatronic design example.
 *
 * This example demonstrates named indexing into declaration-ordered callback
 * spans, opposing objective senses, nonnegative engineering margins, automatic
 * algorithm selection, and a final Boolean system-level validation gate.
 */

namespace {

// Parameter callbacks and best_design.parameters use this declaration order.
constexpr std::size_t gear_teeth_parameter_index = 0;
constexpr std::size_t motor_current_parameter_index = 1;
constexpr std::size_t arm_length_parameter_index = 2;

// Validation callbacks and best_design.objectives use objective declaration order.
constexpr std::size_t moving_mass_objective_index = 0;
constexpr std::size_t payload_objective_index = 1;
constexpr std::size_t declared_objective_count = payload_objective_index + 1;

// The validator receives restriction results in this same declaration order.
constexpr std::size_t thermal_margin_restriction_index = 0;
constexpr std::size_t tip_speed_margin_restriction_index = 1;
constexpr std::size_t structural_payload_margin_restriction_index = 2;
constexpr std::size_t declared_restriction_count = structural_payload_margin_restriction_index + 1;

static_assert(tip_speed_margin_restriction_index == thermal_margin_restriction_index + 1);

} // namespace

int main() {
    using namespace multobjopt;

    // Select a quantized gearbox and arm geometry. The shared formulas stand in
    // for a simulation that could instead be captured by each callable. A
    // scalar_view is temporary and read-only, so this helper reads it during the
    // call and never retains the span, its iterators, or element addresses.
    const auto payload = [](scalar_view values) {
        const scalar gear_teeth = values[gear_teeth_parameter_index];
        const scalar motor_current = values[motor_current_parameter_index];
        const scalar arm_length = values[arm_length_parameter_index];
        return 0.018 * gear_teeth * motor_current / arm_length;
    };

    problem problem_definition;
    // Declaration order defines every parameter callback and result index. The
    // positive resolutions also place all three variables on lower-bound-
    // anchored engineering grids: integral teeth, 0.5 A, and 0.01 m.
    problem_definition.add_parameter("gear_teeth", 12.0, 60.0, 1.0)
        .add_parameter("motor_current", 2.0, 16.0, 0.5)
        .add_parameter("arm_length", 0.20, 0.80, 0.01)
        .add_objective("moving_mass", objective_sense::minimize,
                       [](scalar_view values) {
                           return 1.0 + 0.018 * values[gear_teeth_parameter_index] +
                                  0.060 * values[motor_current_parameter_index] +
                                  1.2 * values[arm_length_parameter_index];
                       })
        .add_objective("payload", objective_sense::maximize, payload)
        // Objective senses affect Pareto dominance: less mass and more payload
        // are better. Both omitted weights default to 1.0 for selecting one
        // scalar compromise; raw result values keep their original senses.
        .add_restriction("thermal_margin",
                         [](scalar_view values) {
                             const scalar current = values[motor_current_parameter_index];
                             return 130.0 - 0.55 * current * current;
                         })
        .add_restriction("tip_speed_margin",
                         [](scalar_view values) {
                             return 1.8 - 0.05 * values[gear_teeth_parameter_index] *
                                              values[arm_length_parameter_index];
                         })
        .add_restriction("structural_payload_margin",
                         [payload](scalar_view values) { return 35.0 - payload(values); })
        .set_validation([](scalar_view objectives, scalar_view restrictions) {
            // This final coupled gate runs only after all objective callbacks and
            // all scalar restriction callbacks. The two temporary spans contain
            // their raw results in declaration order; they must not be retained.
            //
            // Guard the index contract explicitly. The problem definition
            // guarantees these sizes, while the check makes copied/adapted code
            // fail closed if declarations and index constants drift apart.
            if (objectives.size() != declared_objective_count ||
                restrictions.size() != declared_restriction_count) {
                return false;
            }

            const scalar moving_mass = objectives[moving_mass_objective_index];
            const scalar available_payload = objectives[payload_objective_index];
            const scalar thermal_margin = restrictions[thermal_margin_restriction_index];
            const scalar speed_margin = restrictions[tip_speed_margin_restriction_index];
            const scalar structural_margin =
                restrictions[structural_payload_margin_restriction_index];

            // Imagine that a product catalog approves either of these complete
            // motor/arm packages. This disjoint, cross-result lookup is a useful
            // Boolean gate: inventing one continuous "distance" between the two
            // approved packages would give the optimizer misleading physics.
            const bool lightweight_package = moving_mass <= 2.6 && available_payload >= 3.0 &&
                                             thermal_margin >= 20.0 && speed_margin >= 0.25 &&
                                             structural_margin >= 15.0;
            const bool high_payload_package = moving_mass <= 3.5 && available_payload >= 8.0 &&
                                              thermal_margin >= 10.0 && speed_margin >= 0.10 &&
                                              structural_margin >= 5.0;

            // A false result vetoes feasibility and contributes one unit of
            // violation when the numerical evaluation is otherwise valid.
            // Returning true still cannot excuse a negative scalar margin.
            return lightweight_package || high_payload_package;
        });
    // Each restriction above is written as capacity minus demand. Values >= 0
    // are feasible margins; negative values measure violation of design space.

    optimizer_options options;
    // automatic asks the documented structural heuristic to choose a method.
    // The mixed resolutions, opposing objectives, and constraints make this a
    // useful demonstration of inspecting result.algorithm_used rather than
    // assuming which optimizer ran.
    options.algorithm = optimization_algorithm::automatic;
    options.population_size = 64;
    options.max_evaluations = 5'000;
    options.stall_iterations = 120;

    const auto result = optimize(problem_definition, options);
    // best_design is the feasibility-first, weighted scalar compromise. Its
    // objectives are raw values (so larger payload remains desirable), while
    // pareto_front contains feasible nondominated alternatives in deterministic
    // order.
    const auto& design = result.best_design;

    std::cout << "selected algorithm: " << to_string(result.algorithm_used) << '\n'
              << "termination: " << to_string(result.reason) << '\n'
              << "gear teeth: " << design.parameters[gear_teeth_parameter_index] << '\n'
              << "motor current: " << design.parameters[motor_current_parameter_index] << '\n'
              << "arm length: " << design.parameters[arm_length_parameter_index] << '\n'
              << "moving mass: " << design.objectives[moving_mass_objective_index] << '\n'
              << "payload: " << design.objectives[payload_objective_index] << '\n'
              << "thermal margin: " << design.restrictions[thermal_margin_restriction_index] << '\n'
              << "tip-speed margin: " << design.restrictions[tip_speed_margin_restriction_index]
              << '\n'
              << "structural margin: "
              << design.restrictions[structural_payload_margin_restriction_index] << '\n'
              << "valid: " << std::boolalpha << design.valid << '\n'
              << "feasible: " << design.feasible << '\n'
              << "total violation: " << design.total_violation << '\n'
              << "pareto designs: " << result.pareto_front.size() << '\n';
}
