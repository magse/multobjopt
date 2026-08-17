#include <multobjopt/problem.hpp>

#include "test_support.hpp"

int main() {
    test_support::test_context test;

    multobjopt::problem problem;
    problem.add_parameter("x", 0.0, 1.0)
        .add_objective("x", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0]; });
    problem.validate();

    test.check(problem.parameters().size() == 1,
               "problem.hpp independently exposes problem construction");
    return test.finish();
}
