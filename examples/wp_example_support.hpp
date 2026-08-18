#ifndef MULTOBJOPT_EXAMPLES_WP_EXAMPLE_SUPPORT_HPP
#define MULTOBJOPT_EXAMPLES_WP_EXAMPLE_SUPPORT_HPP

#include <multobjopt/multobjopt.hpp>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

/**
 * @file wp_example_support.hpp
 * @brief Small runner shared by the Wikipedia benchmark examples.
 *
 * The benchmark formula itself remains in its numbered source file. This
 * header only supplies deterministic solver settings, an optional check at a
 * published reference design, and uniform result output. Keeping those details
 * here makes every benchmark source short enough to read as a mathematical
 * problem definition.
 */

namespace multobjopt_examples {

/** @brief A design whose published objective values can be checked directly. */
struct reference_design {
    /** Parameter values in the benchmark's declared order. */
    std::vector<multobjopt::scalar> parameters;

    /** Expected raw objective values in declaration order. */
    std::vector<multobjopt::scalar> objectives;

    /** Relative-and-absolute tolerance used for every expected value. */
    multobjopt::scalar tolerance{1.0e-8};

    /** Whether the reference design must satisfy all declared restrictions. */
    bool must_be_feasible{true};
};

/** @brief Execution controls used by one benchmark demonstration. */
struct benchmark_configuration {
    /** Concrete optimizer used for a comparable, deterministic demonstration. */
    multobjopt::optimization_algorithm algorithm{
        multobjopt::optimization_algorithm::genetic_algorithm};

    /** Complete model evaluations allowed for the short example run. */
    std::size_t max_evaluations{1'200};

    /** Outer iterations allowed before the example stops. */
    std::size_t max_iterations{200};

    /** Consecutive non-improving iterations accepted by the solver. */
    std::size_t stall_iterations{60};

    /** Population size used when the genetic algorithm is selected. */
    std::size_t population_size{36};

    /** Optional search starting point; an empty vector selects the midpoint. */
    std::vector<multobjopt::scalar> initial_guess;

    /** Optional independently evaluated reference design. */
    std::optional<reference_design> reference;
};

/**
 * @brief Run and report one optimization benchmark.
 * @param benchmark_name Human-readable benchmark name.
 * @param problem_definition Fully configured multobjopt problem.
 * @param configuration Short-run controls and optional reference check.
 * @return `EXIT_SUCCESS` when evaluation and optional reference checks pass.
 *
 * These executables demonstrate problem transcription rather than rank solver
 * performance. A modest, fixed budget keeps the complete CTest suite quick.
 * The optional reference is evaluated independently, so the example verifies
 * its formula without seeding the optimizer at the published answer.
 */
inline int run_benchmark(std::string_view benchmark_name,
                         const multobjopt::problem& problem_definition,
                         benchmark_configuration configuration = {}) {
    using multobjopt::scalar;

    bool reference_matches = true;
    if (configuration.reference.has_value()) {
        const reference_design& expected = *configuration.reference;
        const auto actual = multobjopt::evaluate_design(problem_definition, expected.parameters);
        reference_matches = actual.valid &&
                            actual.objectives.size() == expected.objectives.size() &&
                            (!expected.must_be_feasible || actual.feasible);

        if (reference_matches) {
            for (std::size_t index = 0; index < expected.objectives.size(); ++index) {
                const scalar scale = std::max<scalar>(1.0, std::abs(expected.objectives[index]));
                if (std::abs(actual.objectives[index] - expected.objectives[index]) >
                    expected.tolerance * scale) {
                    reference_matches = false;
                    break;
                }
            }
        }

        std::cout << "reference check: " << (reference_matches ? "passed" : "FAILED") << '\n';
    }

    multobjopt::optimizer_options options;
    options.algorithm = configuration.algorithm;
    options.max_evaluations = configuration.max_evaluations;
    options.max_iterations = configuration.max_iterations;
    options.stall_iterations = configuration.stall_iterations;
    options.population_size = configuration.population_size;
    options.initial_guess = std::move(configuration.initial_guess);

    const auto result = multobjopt::optimize(problem_definition, options);
    const auto& design = result.best_design;

    std::cout << std::setprecision(10) << "benchmark: " << benchmark_name << '\n'
              << "algorithm: " << multobjopt::to_string(result.algorithm_used) << '\n'
              << "termination: " << multobjopt::to_string(result.reason) << '\n'
              << "evaluations: " << result.evaluations << '\n'
              << "valid: " << std::boolalpha << design.valid << '\n'
              << "feasible: " << design.feasible << '\n'
              << "parameters:";
    for (const scalar value : design.parameters) {
        std::cout << ' ' << value;
    }
    std::cout << "\nobjectives:";
    for (const scalar value : design.objectives) {
        std::cout << ' ' << value;
    }
    std::cout << "\nrestrictions:";
    for (const scalar value : design.restrictions) {
        std::cout << ' ' << value;
    }
    std::cout << "\npareto designs: " << result.pareto_front.size() << '\n';

    return reference_matches && design.valid ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // namespace multobjopt_examples

#endif
