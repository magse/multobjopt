#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <cstddef>
#include <string>

int main() {
    test_support::test_context test;
    bool callback_dimension_is_correct = true;
    multobjopt::problem problem;
    for (int index = 0; index < 25; ++index) {
        problem.add_parameter("x" + std::to_string(index), -1.0, 1.0);
    }
    problem.add_objective(
        "sphere", multobjopt::objective_sense::minimize, [&](multobjopt::scalar_view values) {
            callback_dimension_is_correct = callback_dimension_is_correct && values.size() == 25;
            double sum = 0.0;
            for (const double value : values) {
                sum += value * value;
            }
            return sum;
        });
    problem.validate();

    multobjopt::optimizer_options options;
    options.algorithm = multobjopt::optimization_algorithm::automatic;
    options.max_evaluations = 40;
    options.max_iterations = 4;
    options.stall_iterations = 10;
    options.moves_per_temperature = 10;
    options.random_seed = 12'525;
    const auto result = multobjopt::optimize(problem, options);

    test.check(callback_dimension_is_correct, "callbacks receive all twenty-five parameter values");
    test.check(result.best_design.parameters.size() == 25,
               "the result retains all twenty-five parameters");
    test.check(result.algorithm_used == multobjopt::optimization_algorithm::simulated_annealing,
               "automatic selection uses annealing above twelve continuous dimensions");
    test.check_near(result.best_design.objectives[0], 0.0, 0.0,
                    "the midpoint finds the sphere optimum");
    test.check(result.evaluations <= options.max_evaluations,
               "the high-dimensional smoke test respects its small budget");

    return test.finish();
}
