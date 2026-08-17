#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <vector>

int main() {
    test_support::test_context test;
    bool all_evaluations_in_bounds = true;

    multobjopt::problem problem;
    problem.add_parameter("x", -3.0, 3.0)
        .add_parameter("y", -3.0, 3.0)
        .add_objective("quadratic", multobjopt::objective_sense::minimize,
                       [&](multobjopt::scalar_view values) {
                           all_evaluations_in_bounds = all_evaluations_in_bounds &&
                                                       values[0] >= -3.0 && values[0] <= 3.0 &&
                                                       values[1] >= -3.0 && values[1] <= 3.0;
                           const double x_error = values[0] - 1.25;
                           const double y_error = values[1] + 0.75;
                           return x_error * x_error + 2.0 * y_error * y_error;
                       });

    multobjopt::optimizer_options options;
    options.algorithm = multobjopt::optimization_algorithm::gradient_descent;
    options.max_evaluations = 500;
    options.max_iterations = 150;
    options.stall_iterations = 80;
    options.initial_guess = {-2.0, 2.0};
    options.initial_step_scale = 0.25;
    options.gradient_tolerance = 1.0e-6;

    const auto result = multobjopt::optimize(problem, options);
    test.check(result.algorithm_used == multobjopt::optimization_algorithm::gradient_descent,
               "the requested gradient method is reported");
    test.check(result.evaluations <= options.max_evaluations,
               "gradient descent respects the evaluation budget");
    test.check(all_evaluations_in_bounds,
               "numerical gradients are evaluated inside parameter bounds");
    test.check(result.best_design.feasible, "gradient descent returns a feasible design");
    test.check(result.best_design.objectives[0] <= 1.0e-6,
               "gradient descent converges on a smooth quadratic");

    bool boundary_evaluations_in_bounds = true;
    multobjopt::problem boundary_problem;
    boundary_problem.add_parameter("x", 0.0, 3.0)
        .add_objective("boundary_quadratic", multobjopt::objective_sense::minimize,
                       [&](multobjopt::scalar_view values) {
                           boundary_evaluations_in_bounds = boundary_evaluations_in_bounds &&
                                                            values[0] >= 0.0 && values[0] <= 3.0;
                           const double error = values[0] + 1.0;
                           return error * error;
                       });
    auto boundary_options = options;
    boundary_options.initial_guess = {2.0};
    const auto boundary_result = multobjopt::optimize(boundary_problem, boundary_options);
    test.check(boundary_evaluations_in_bounds,
               "one-sided numerical differences stay inside bounds");
    test.check_near(boundary_result.best_design.parameters[0], 0.0, 1.0e-6,
                    "projected gradient descent reaches a boundary optimum");

    return test.finish();
}
