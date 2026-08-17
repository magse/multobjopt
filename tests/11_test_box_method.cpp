#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

int main() {
    test_support::test_context test;
    bool all_evaluations_in_bounds = true;

    multobjopt::problem problem;
    problem.add_parameter("x", 0.0, 4.0)
        .add_parameter("y", 0.0, 4.0)
        .add_objective("distance", multobjopt::objective_sense::minimize,
                       [&](multobjopt::scalar_view values) {
                           all_evaluations_in_bounds = all_evaluations_in_bounds &&
                                                       values[0] >= 0.0 && values[0] <= 4.0 &&
                                                       values[1] >= 0.0 && values[1] <= 4.0;
                           const double x_error = values[0] - 2.0;
                           const double y_error = values[1] - 2.0;
                           return x_error * x_error + y_error * y_error;
                       })
        .add_restriction("sum_limit", [](multobjopt::scalar_view values) {
            return 3.0 - values[0] - values[1];
        });

    multobjopt::optimizer_options options;
    options.algorithm = multobjopt::optimization_algorithm::box_method;
    options.max_evaluations = 600;
    options.max_iterations = 200;
    options.stall_iterations = 60;
    options.random_seed = 71'003;
    options.initial_guess = {1.0, 1.0};
    options.complex_size = 8;

    const auto result = multobjopt::optimize(problem, options);
    test.check(result.algorithm_used == multobjopt::optimization_algorithm::box_method,
               "the requested Box method is reported");
    test.check(result.evaluations <= options.max_evaluations,
               "the Box method respects the evaluation budget");
    test.check(all_evaluations_in_bounds,
               "the Box method never evaluates outside parameter bounds");
    test.check(result.best_design.feasible, "the Box method returns a feasible constrained design");
    test.check(result.best_design.restrictions[0] >= -options.constraint_tolerance,
               "the returned design satisfies the sum restriction");
    test.check(result.best_design.objectives[0] <= 1.0,
               "the Box method improves toward the constrained optimum");

    return test.finish();
}
