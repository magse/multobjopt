#include <multobjopt/optimize.hpp>

#include "test_support.hpp"

int main() {
    test_support::test_context test;

    multobjopt::problem problem;
    problem.add_parameter("fixed", 0.5, 0.5)
        .add_objective("value", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0]; });

    multobjopt::optimizer_options options;
    options.algorithm = multobjopt::optimization_algorithm::gradient_descent;
    const auto result = multobjopt::optimize(problem, options);

    test.check(result.best_design.feasible && result.evaluations == 1,
               "optimize.hpp independently exposes the complete solve entry point");
    return test.finish();
}
