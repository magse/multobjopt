#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

int main() {
    test_support::test_context test;

    multobjopt::problem searchable;
    searchable.add_parameter("x", 0.0, 1.0)
        .add_objective("target", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) {
                           const double error = values[0] - 0.8;
                           return error * error;
                       })
        .add_restriction("upper_region",
                         [](multobjopt::scalar_view values) { return values[0] - 0.7; });
    multobjopt::optimizer_options options;
    options.algorithm = multobjopt::optimization_algorithm::box_method;
    options.initial_guess = {0.1};
    options.random_seed = 24'003;
    options.complex_size = 5;
    options.max_evaluations = 300;
    options.stall_iterations = 50;
    const auto found = multobjopt::optimize(searchable, options);
    test.check(found.best_design.feasible,
               "Box searches for a feasible seed when the initial guess violates");
    test.check(found.best_design.parameters[0] >= 0.7,
               "the repaired Box complex respects the nonlinear restriction");

    multobjopt::problem impossible;
    impossible.add_parameter("x", 0.0, 1.0)
        .add_objective("x", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0]; })
        .add_restriction("impossible",
                         [](multobjopt::scalar_view values) { return values[0] - 2.0; });
    options.max_evaluations = 20;
    options.initial_guess.clear();
    const auto not_found = multobjopt::optimize(impossible, options);
    test.check(!not_found.best_design.feasible && not_found.pareto_front.empty(),
               "an impossible Box problem has no feasible output");
    test.check(not_found.reason == multobjopt::termination_reason::no_feasible_design,
               "an impossible Box problem has an explicit status");

    return test.finish();
}
