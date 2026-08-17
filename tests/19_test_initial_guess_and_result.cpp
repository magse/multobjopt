#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <limits>
#include <stdexcept>

int main() {
    test_support::test_context test;
    multobjopt::problem problem;
    problem.add_parameter("x", -2.0, 2.0, 0.5)
        .add_objective("square", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0] * values[0]; })
        .add_restriction("upper_margin",
                         [](multobjopt::scalar_view values) { return 2.0 - values[0]; });

    multobjopt::optimizer_options options;
    options.algorithm = multobjopt::optimization_algorithm::simulated_annealing;
    options.max_evaluations = 1;
    options.max_iterations = 5;
    options.initial_guess = {9.0};
    const auto result = multobjopt::optimize(problem, options);

    test.check(result.algorithm_used == options.algorithm,
               "an explicit algorithm overrides automatic selection");
    test.check(result.evaluations == 1, "the initial design counts as one evaluation");
    test.check(result.iterations == 1, "the annealing run records the budget-ending iteration");
    test.check(result.reason == multobjopt::termination_reason::evaluation_limit,
               "an exhausted one-evaluation budget is reported");
    test.check(result.best_design.parameters == std::vector<double>({2.0}),
               "the initial guess is clamped and quantized before evaluation");
    test.check(result.best_design.objectives == std::vector<double>({4.0}),
               "result metadata retains objective values");
    test.check(result.best_design.restrictions == std::vector<double>({0.0}),
               "result metadata retains restriction values");
    test.check(result.best_design.feasible, "the normalized initial design is feasible");
    test.check(result.pareto_front.size() == 1,
               "the feasible initial design is entered in the archive");

    auto wrong_size = options;
    wrong_size.initial_guess = {0.0, 1.0};
    test.check_throws<std::invalid_argument>(
        [&] { static_cast<void>(multobjopt::optimize(problem, wrong_size)); },
        "initial guess dimension is validated");

    auto nonfinite = options;
    nonfinite.initial_guess = {std::numeric_limits<double>::quiet_NaN()};
    test.check_throws<std::invalid_argument>(
        [&] { static_cast<void>(multobjopt::optimize(problem, nonfinite)); },
        "initial guess values must be finite");

    return test.finish();
}
