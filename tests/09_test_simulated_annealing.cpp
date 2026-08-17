#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <cmath>

int main() {
    test_support::test_context test;
    bool all_evaluations_on_grid = true;

    multobjopt::problem problem;
    problem.add_parameter("x", -5.0, 5.0, 0.5)
        .add_objective("distance", multobjopt::objective_sense::minimize,
                       [&](multobjopt::scalar_view values) {
                           const double grid_index = (values[0] + 5.0) / 0.5;
                           all_evaluations_on_grid =
                               all_evaluations_on_grid && values[0] >= -5.0 && values[0] <= 5.0 &&
                               std::abs(grid_index - std::round(grid_index)) < 1.0e-12;
                           const double error = values[0] - 1.5;
                           return error * error;
                       });

    multobjopt::optimizer_options options;
    options.algorithm = multobjopt::optimization_algorithm::simulated_annealing;
    options.max_evaluations = 600;
    options.max_iterations = 80;
    options.stall_iterations = 100;
    options.random_seed = 10'901;
    options.initial_guess = {-4.0};
    options.initial_temperature = 4.0;
    options.final_temperature = 1.0e-3;
    options.cooling_rate = 0.88;
    options.annealing_step_scale = 0.30;
    options.moves_per_temperature = 8;

    const auto result = multobjopt::optimize(problem, options);
    test.check(result.algorithm_used == multobjopt::optimization_algorithm::simulated_annealing,
               "the requested simulated annealing method is reported");
    test.check(result.evaluations > 1 && result.evaluations <= options.max_evaluations,
               "annealing respects the evaluation budget");
    test.check(result.best_design.feasible, "annealing returns a feasible design");
    test.check(all_evaluations_on_grid, "annealing evaluates only bounded, quantized designs");
    test.check(result.best_design.objectives[0] <= 0.25,
               "annealing finds the neighborhood of the known optimum");
    test.check(!result.pareto_front.empty(), "annealing records a nondominated feasible design");

    return test.finish();
}
