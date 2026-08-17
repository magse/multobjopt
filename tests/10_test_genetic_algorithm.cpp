#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <cmath>

int main() {
    test_support::test_context test;
    bool all_evaluations_are_valid = true;

    multobjopt::problem problem;
    problem.add_parameter("x", -4.0, 4.0, 0.5)
        .add_parameter("y", -4.0, 4.0, 0.5)
        .add_objective("distance", multobjopt::objective_sense::minimize,
                       [&](multobjopt::scalar_view values) {
                           for (const double value : values) {
                               const double grid_index = (value + 4.0) / 0.5;
                               all_evaluations_are_valid =
                                   all_evaluations_are_valid && value >= -4.0 && value <= 4.0 &&
                                   std::abs(grid_index - std::round(grid_index)) < 1.0e-12;
                           }
                           const double x_error = values[0] - 1.0;
                           const double y_error = values[1] + 2.0;
                           return x_error * x_error + y_error * y_error;
                       });

    multobjopt::optimizer_options options;
    options.algorithm = multobjopt::optimization_algorithm::genetic_algorithm;
    options.max_evaluations = 640;
    options.max_iterations = 30;
    options.stall_iterations = 40;
    options.random_seed = 22'817;
    options.initial_guess = {-4.0, 4.0};
    options.population_size = 32;
    options.elite_count = 2;
    options.tournament_size = 3;
    options.mutation_rate = 0.30;
    options.mutation_scale = 0.20;

    const auto result = multobjopt::optimize(problem, options);
    test.check(result.algorithm_used == multobjopt::optimization_algorithm::genetic_algorithm,
               "the requested genetic algorithm is reported");
    test.check(result.evaluations <= options.max_evaluations,
               "the genetic algorithm respects the evaluation budget");
    test.check(result.best_design.feasible, "the genetic algorithm returns a feasible design");
    test.check(all_evaluations_are_valid, "the genetic algorithm evaluates bounded grid points");
    test.check(result.best_design.objectives[0] <= 0.25,
               "the genetic algorithm finds the known grid optimum neighborhood");
    test.check(!result.pareto_front.empty(), "the genetic algorithm maintains a feasible archive");

    return test.finish();
}
