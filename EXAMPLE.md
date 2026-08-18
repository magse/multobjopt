# Developing optimization models with multobjopt

This guide develops two complete C++20 programs: a minimal bounded quadratic
and a coupled mechatronic design model. It concentrates on translating a model
into callbacks and interpreting the resulting records. See [README.md](README.md)
for the library's contracts and algorithm-selection policy, and
[INSTALL.md](INSTALL.md) for installation and downstream CMake setup.

Shorter repository programs are available in
[examples/01_basic_quadratic.cpp](examples/01_basic_quadratic.cpp) and
[examples/04_mechatronic_discrete_auto.cpp](examples/04_mechatronic_discrete_auto.cpp).
The programs below are deliberately more explanatory and can each be copied
into a separate `main.cpp` and linked to `multobjopt::multobjopt`.

## 1. A minimal bounded quadratic

A `problem` owns its definitions and callbacks. Parameters are supplied to an
objective in the order in which they were added. This example has two
continuous parameters, one objective, no restrictions, and no overall
validator. Automatic selection therefore chooses numerical gradient descent.

```cpp
#include <multobjopt/multobjopt.hpp>

#include <cstdlib>
#include <iostream>

int main() {
    using namespace multobjopt;

    problem problem_definition;
    problem_definition
        // A zero resolution, which is the default, makes each parameter
        // continuous within its inclusive bounds.
        .add_parameter("x", -5.0, 5.0)
        .add_parameter("y", -5.0, 5.0)
        .add_objective(
            "squared_error",
            objective_sense::minimize,
            [](scalar_view parameters) {
                // Parameter indexes follow add_parameter() declaration order.
                const scalar x_error = parameters[0] - 1.5;
                const scalar y_error = parameters[1] + 0.5;
                return x_error * x_error + y_error * y_error;
            });

    optimizer_options options;
    options.algorithm = optimization_algorithm::automatic;
    options.initial_guess = {-3.0, 4.0};
    options.max_evaluations = 2'000;
    options.max_iterations = 500;

    const optimization_result result = optimize(problem_definition, options);
    const evaluated_design& best = result.best_design;

    std::cout << "algorithm: " << to_string(result.algorithm_used) << '\n'
              << "termination: " << to_string(result.reason) << '\n'
              << "evaluations: " << result.evaluations << '\n'
              << "x: " << best.parameters[0] << '\n'
              << "y: " << best.parameters[1] << '\n'
              << "raw objective: " << best.objectives[0] << '\n'
              << "valid: " << std::boolalpha << best.valid << '\n'
              << "feasible: " << best.feasible << '\n';

    // With no restrictions or validator, every numerically valid design is
    // feasible. The known solution is (1.5, -0.5), with objective value zero.
    return best.valid && best.feasible ? EXIT_SUCCESS : EXIT_FAILURE;
}
```

The optimizer clamps every candidate to the declared bounds before invoking
the objective. `best.objectives[0]` is the raw callback value; the owning result
vectors remain valid for the lifetime of `result`. The selected algorithm and
termination reason are reported explicitly rather than inferred from the
requested `automatic` value.

## 2. A coupled mechatronic design

The next program sizes a geared rotary arm. It combines two grid-valued
component choices with two continuous dimensions:

- `gear_ratio` and `motor_current` are quantized to purchasable increments;
- `arm_length` and `tube_radius` are continuous;
- moving mass is minimized while payload capacity is maximized;
- thermal, speed, and structural margins are ordinary scalar restrictions;
- a final Boolean validator applies coupled system-acceptance rules.

The callbacks capture one shared, immutable model object. A production model
could call a finite-element solver, load a motor map, or share cached immutable
data in the same way. This example recalculates a small analytic model so that
the complete program remains self-contained.

### The overall validator is the final system-level gate

`set_validation()` is intentionally different from a scalar restriction. It
runs only after all objectives and restrictions have returned and receives two
temporary spans containing their raw values in declaration order. It receives
values, not the original parameter vector, and returns only `bool`.

That makes the validator useful for coupled acceptance logic, such as selecting
one of several approved mass, payload, and safety-margin packages. It also
imposes important limits:

- it cannot express how far a rejected design is from acceptance;
- rejection adds one unit to `total_violation` for an otherwise finite
  evaluation;
- it does not replace scalar restrictions, whose values and scales give the
  optimizers useful violation-distance information;
- every callback's `scalar_view` arguments are temporary and must never be
  retained;
- it may still be called with a non-finite raw value, which cannot be rescued by
  returning `true`; check finiteness before doing unsafe arithmetic when a
  scalar model callback can produce such a value;
- exceptions from objectives, restrictions, or the validator propagate through
  `evaluate_design()` and `optimize()` to the caller.

Use scalar restrictions for individual measurable boundaries and reserve the
validator for the final whole-system decision.

```cpp
#include <multobjopt/multobjopt.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string_view>

namespace {

// Named indices keep every callback synchronized with declaration order. When
// adapting the example, change the declarations and these constants together.
constexpr std::size_t gear_ratio_parameter_index = 0;
constexpr std::size_t motor_current_parameter_index = 1;
constexpr std::size_t arm_length_parameter_index = 2;
constexpr std::size_t tube_radius_parameter_index = 3;
constexpr std::size_t declared_parameter_count = 4;

constexpr std::size_t moving_mass_objective_index = 0;
constexpr std::size_t payload_capacity_objective_index = 1;
constexpr std::size_t declared_objective_count = 2;

constexpr std::size_t thermal_margin_restriction_index = 0;
constexpr std::size_t speed_margin_restriction_index = 1;
constexpr std::size_t structural_margin_restriction_index = 2;
constexpr std::size_t declared_restriction_count = 3;

/** Outputs produced together by the shared engineering model. */
struct actuator_metrics {
    multobjopt::scalar moving_mass_kg;
    multobjopt::scalar payload_capacity_kg;
    multobjopt::scalar winding_temperature_c;
    multobjopt::scalar tip_speed_m_per_s;
    multobjopt::scalar bending_stress_mpa;
};

/** Small stand-in for a coupled motor, gearbox, and arm simulation. */
class actuator_model {
  public:
    [[nodiscard]] actuator_metrics operator()(multobjopt::scalar_view parameters) const {
        using multobjopt::scalar;

        const scalar gear_ratio = parameters[gear_ratio_parameter_index];
        const scalar motor_current = parameters[motor_current_parameter_index];
        const scalar arm_length = parameters[arm_length_parameter_index];
        const scalar tube_radius = parameters[tube_radius_parameter_index];

        // A compact analytic model with caller-selected engineering units.
        // The constants are illustrative rather than a certified machine model.
        const scalar gearbox_efficiency = 0.85;
        const scalar output_torque_nm =
            0.35 * motor_current * gear_ratio * gearbox_efficiency;
        const scalar payload_capacity_kg = output_torque_nm / (9.81 * arm_length);

        const scalar tube_mass_kg =
            600.0 * tube_radius * tube_radius * arm_length;
        const scalar moving_mass_kg =
            1.0 + 0.08 * gear_ratio + 0.04 * motor_current + tube_mass_kg;

        const scalar winding_temperature_c =
            30.0 + 0.85 * motor_current * motor_current;
        const scalar tip_speed_m_per_s = 12.0 * arm_length / gear_ratio;

        const scalar radius_cubed =
            tube_radius * tube_radius * tube_radius;
        const scalar bending_stress_mpa =
            0.001 * payload_capacity_kg * arm_length / radius_cubed;

        return {moving_mass_kg, payload_capacity_kg, winding_temperature_c,
                tip_speed_m_per_s, bending_stress_mpa};
    }
};

/** Print owning raw and derived values from one completed evaluation. */
void print_design(std::string_view label,
                  const multobjopt::problem& problem_definition,
                  const multobjopt::evaluated_design& design) {
    std::cout << label << '\n'
              << "  valid: " << std::boolalpha << design.valid << '\n'
              << "  feasible: " << design.feasible << '\n'
              << "  total violation: " << design.total_violation << '\n'
              << "  scalarized objective: " << design.scalarized_objective << '\n';

    for (std::size_t index = 0; index < design.parameters.size(); ++index) {
        std::cout << "  parameter "
                  << problem_definition.parameters()[index].name << ": "
                  << design.parameters[index] << '\n';
    }
    for (std::size_t index = 0; index < design.objectives.size(); ++index) {
        std::cout << "  raw objective "
                  << problem_definition.objectives()[index].name << ": "
                  << design.objectives[index] << '\n';
    }
    for (std::size_t index = 0; index < design.restrictions.size(); ++index) {
        std::cout << "  raw restriction "
                  << problem_definition.restrictions()[index].name << ": "
                  << design.restrictions[index] << '\n';
    }
}

} // namespace

int main() {
    using namespace multobjopt;

    // Every callback captures the same immutable model. Capturing a shared
    // object is useful when several outputs come from one simulation codebase.
    const auto model = std::make_shared<const actuator_model>();

    problem problem_definition;
    problem_definition
        // Positive resolutions define grids anchored at each lower bound.
        .add_parameter("gear_ratio", 4.0, 12.0, 0.5)
        .add_parameter("motor_current", 2.0, 12.0, 0.25)
        // Zero resolution selects continuous parameters.
        .add_parameter("arm_length", 0.25, 0.75)
        .add_parameter("tube_radius", 0.018, 0.035)
        .add_objective(
            "moving_mass_kg",
            objective_sense::minimize,
            [model](scalar_view parameters) {
                return (*model)(parameters).moving_mass_kg;
            },
            // One score unit per kilogram of moving mass.
            1.0)
        .add_objective(
            "payload_capacity_kg",
            objective_sense::maximize,
            [model](scalar_view parameters) {
                return (*model)(parameters).payload_capacity_kg;
            },
            // Five kilograms of payload offset one kilogram in the scalar
            // compromise: maximize contributes -weight * raw_value.
            0.20)
        .add_restriction(
            "thermal_margin_c",
            [model](scalar_view parameters) {
                // Non-negative means the winding temperature is at most 100 C.
                return 100.0 - (*model)(parameters).winding_temperature_c;
            },
            // Normalize thermal violations by a characteristic 20 C margin.
            20.0)
        .add_restriction(
            "tip_speed_margin_m_per_s",
            [model](scalar_view parameters) {
                // Non-negative means the tip reaches at least 0.45 m/s.
                return (*model)(parameters).tip_speed_m_per_s - 0.45;
            },
            0.25)
        .add_restriction(
            "structural_margin_mpa",
            [model](scalar_view parameters) {
                // Non-negative means bending stress does not exceed 220 MPa.
                return 220.0 - (*model)(parameters).bending_stress_mpa;
            },
            50.0)
        .set_validation(
            [](scalar_view objectives, scalar_view restrictions) {
                // The library calls this last. These are raw callback values,
                // in exactly the objective/restriction declaration order above.
                if (objectives.size() != declared_objective_count ||
                    restrictions.size() != declared_restriction_count) {
                    return false;
                }

                const scalar moving_mass_kg =
                    objectives[moving_mass_objective_index];
                const scalar payload_capacity_kg =
                    objectives[payload_capacity_objective_index];
                const scalar thermal_margin_c =
                    restrictions[thermal_margin_restriction_index];
                const scalar speed_margin_m_per_s =
                    restrictions[speed_margin_restriction_index];
                const scalar structural_margin_mpa =
                    restrictions[structural_margin_restriction_index];

                // A catalog permits either complete package. The disjoint
                // alternatives have no honest single distance between them,
                // making this a better Boolean gate than scalar restriction.
                const bool lightweight_package =
                    moving_mass_kg <= 2.6 && payload_capacity_kg >= 3.2 &&
                    thermal_margin_c >= 20.0 &&
                    speed_margin_m_per_s >= 0.15 &&
                    structural_margin_mpa >= 50.0;
                const bool high_capacity_package =
                    moving_mass_kg <= 3.4 && payload_capacity_kg >= 6.0 &&
                    thermal_margin_c >= 10.0 &&
                    speed_margin_m_per_s >= 0.05 &&
                    structural_margin_mpa >= 25.0;

                return lightweight_package || high_capacity_package;
            });

    // Evaluate one known candidate before starting a search. Values are first
    // clamped and quantized, so 8.1 becomes 8.0 and 7.12 becomes 7.0.
    const std::array<scalar, declared_parameter_count> trial_parameters{
        8.1, 7.12, 0.45, 0.025};
    const evaluated_design sanity_design =
        evaluate_design(problem_definition, trial_parameters);
    print_design("direct sanity evaluation", problem_definition, sanity_design);

    if (!sanity_design.valid || !sanity_design.feasible) {
        std::cerr << "the model sanity design did not pass its declared gates\n";
        return EXIT_FAILURE;
    }

    optimizer_options options;
    options.algorithm = optimization_algorithm::automatic;
    options.random_seed = 20'260'818; // Fixed seed makes stochastic runs repeatable.
    options.initial_guess = sanity_design.parameters;
    options.max_evaluations = 6'000;
    options.max_iterations = 400;
    options.stall_iterations = 100;
    options.population_size = 64;
    options.elite_count = 4;

    // Retaining every completed evaluation is optional and disabled by
    // default. Enable it only when a chronological trace is useful; Pareto and
    // JSON summary reports do not otherwise require the full trajectory.
    options.record_evaluation_history = true;

    // Multiple objectives and quantized parameters cause automatic selection
    // to choose the genetic algorithm. The concrete method is recorded below.
    const optimization_result result = optimize(problem_definition, options);

    std::cout << "\noptimization summary\n"
              << "  algorithm used: " << to_string(result.algorithm_used) << '\n'
              << "  termination: " << to_string(result.reason) << '\n'
              << "  evaluations: " << result.evaluations << '\n'
              << "  iterations: " << result.iterations << '\n'
              << "  Pareto designs: " << result.pareto_front.size() << '\n';
    print_design("best scalar compromise", problem_definition, result.best_design);

    // The Pareto front contains only feasible, nondominated designs. Objective
    // senses define dominance; weights only select result.best_design from the
    // available compromises.
    const std::size_t designs_to_show =
        std::min<std::size_t>(5, result.pareto_front.size());
    for (std::size_t index = 0; index < designs_to_show; ++index) {
        const evaluated_design& design = result.pareto_front[index];
        std::cout << "pareto[" << index << "]: mass = "
                  << design.objectives[moving_mass_objective_index]
                  << " kg, payload = "
                  << design.objectives[payload_capacity_objective_index]
                  << " kg, gear ratio = "
                  << design.parameters[gear_ratio_parameter_index] << '\n';
    }

    // A valid but infeasible design has finite model outputs but violates a
    // restriction or the validator. A successful engineering result needs both.
    return result.best_design.valid && result.best_design.feasible
               ? EXIT_SUCCESS
               : EXIT_FAILURE;
}
```

### Reading the second result

The direct `evaluate_design()` call is a useful model-level test: it exercises
normalization, every scalar callback, callback ordering, and the overall
validator without spending an optimization budget. Its result owns the
normalized parameters, raw objective values, and raw restriction values.

During optimization, each complete pass through all those callbacks counts as
one evaluation. `valid` means the model produced finite, representable values;
`feasible` additionally requires every scalar restriction and the validator to
accept the design. Consequently, a finite design rejected by the validator can
remain valid while being infeasible with an additional unit of
`total_violation`.

For this problem, automatic selection chooses the genetic algorithm because
the problem has multiple objectives and quantized parameters. The fixed seed,
initial design, and deterministic result ordering make repeated runs
reproducible. `reason`, `evaluations`, and `iterations` explain how the run
ended. If no feasible design is found, the Pareto front is empty and the final
reason is `no_feasible_design`.

Finally, do not treat the weighted `best_design` as the only meaningful answer
to a multi-objective problem. Its `scalarized_objective` reflects the declared
weights, whereas `pareto_front` preserves the feasible nondominated trade-offs
under the declared minimize/maximize senses. Engineering selection can then
apply review criteria that were intentionally kept outside the optimizer.

### Optional CSV, TSV, and JSON output

The second example enabled `record_evaluation_history` before calling
`optimize()`. Consequently, `result.evaluation_history` owns every completed
evaluation in chronological order. The history includes candidates that were
repeated, infeasible, rejected by the coupled validator, or numerically invalid;
it is not limited to accepted genetic-algorithm generations. Element zero is
evaluation one, and the vector size equals `result.evaluations`.

Reporting remains a deliberate application action after optimization. For
example, add `<fstream>` and write any subset of the available reports:

```cpp
std::ofstream history{"mechatronic_evaluations.tsv"};
multobjopt::write_evaluation_history(
    history, problem_definition, result,
    multobjopt::delimited_text_format::tsv);

std::ofstream pareto{"mechatronic_pareto.tsv"};
multobjopt::write_pareto_front(
    pareto, problem_definition, result,
    multobjopt::delimited_text_format::tsv);

std::ofstream summary{"mechatronic_summary.json"};
multobjopt::write_summary_json(summary, problem_definition, result);
```

Omit `record_evaluation_history` when only the final Pareto front or summary is
needed. Omit any writer call when that report is not wanted. `optimize()` itself
does not create files or print anything. The stream interface also allows an
application to use `std::ostringstream`, a logging stream, or another custom
destination instead of a file.

Table columns contain a one-based row index, normalized parameters, raw
objectives, raw restrictions, `valid`, `feasible`, `total_violation`, and
`scalarized_objective`. The JSON document summarizes the run and names each
best-design value using its corresponding problem definition. This makes the
combined validator's decisions diagnosable: a history row can remain
numerically valid while `feasible` is false and `total_violation` contains its
Boolean-veto contribution.

## Building either program

Use the installed CMake target described in [INSTALL.md](INSTALL.md). For a
source file named `main.cpp`, the essential consumer declarations are:

```cmake
find_package(multobjopt CONFIG REQUIRED)
add_executable(my_design main.cpp)
target_link_libraries(my_design PRIVATE multobjopt::multobjopt)
target_compile_features(my_design PRIVATE cxx_std_20)
```

Both examples use only the public umbrella header and the C++20 standard
library; no additional runtime dependency is required.
