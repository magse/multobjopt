#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <limits>
#include <stdexcept>

int main() {
    test_support::test_context test;

    multobjopt::problem scaled;
    scaled.add_parameter("x", 0.0, 0.0)
        .add_objective("x", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0]; })
        .add_restriction(
            "newtons", [](multobjopt::scalar_view) { return -100.0; }, 100.0)
        .add_restriction("metres", [](multobjopt::scalar_view) { return -1.0; }, 1.0);
    multobjopt::optimizer_options options;
    options.algorithm = multobjopt::optimization_algorithm::gradient_descent;
    const auto scaled_result = multobjopt::optimize(scaled, options);
    test.check(scaled_result.best_design.valid,
               "finite restriction violations remain numerically valid");
    test.check_near(scaled_result.best_design.total_violation, 2.0, 1.0e-12,
                    "restriction scales normalize different engineering units");
    test.check(scaled_result.reason == multobjopt::termination_reason::no_feasible_design,
               "a valid but infeasible fixed design has an explicit status");

    multobjopt::problem invalid_scale;
    invalid_scale.add_parameter("x", 0.0, 1.0)
        .add_objective("x", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0]; })
        .add_restriction("bad_scale", [](multobjopt::scalar_view) { return 0.0; }, 0.0);
    test.check_throws<std::invalid_argument>([&invalid_scale] { invalid_scale.validate(); },
                                             "restriction scales must be positive");

    multobjopt::problem nan_problem;
    nan_problem.add_parameter("x", 0.0, 0.0)
        .add_objective("nan", multobjopt::objective_sense::minimize, [](multobjopt::scalar_view) {
            return std::numeric_limits<double>::quiet_NaN();
        });
    const auto nan_result = multobjopt::optimize(nan_problem, options);
    test.check(!nan_result.best_design.valid,
               "a non-finite callback is distinguished from constraint failure");
    test.check(multobjopt::to_string(multobjopt::termination_reason::no_feasible_design) ==
                   "no_feasible_design",
               "the no-feasible status has a stable name");
    test.check(multobjopt::to_string(multobjopt::termination_reason::numerical_failure) ==
                   "numerical_failure",
               "the numerical-failure status has a stable name");

    return test.finish();
}
