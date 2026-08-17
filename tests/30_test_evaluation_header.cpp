#include <multobjopt/evaluation.hpp>

#include "test_support.hpp"

#include <vector>

int main() {
    test_support::test_context test;

    multobjopt::problem problem;
    problem.add_parameter("x", 0.0, 1.0, 0.25)
        .add_objective("x", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0]; });

    const auto design = multobjopt::evaluate_design(problem, std::vector<double>{0.61});
    test.check(design.parameters[0] == 0.5,
               "evaluation.hpp independently exposes normalization and evaluation");
    return test.finish();
}
