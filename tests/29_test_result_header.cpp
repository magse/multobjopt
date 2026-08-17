#include <multobjopt/result.hpp>

#include "test_support.hpp"

int main() {
    test_support::test_context test;

    multobjopt::evaluated_design design;
    design.parameters = {0.25};
    design.valid = true;
    design.feasible = true;

    multobjopt::optimization_result result;
    result.best_design = design;
    result.pareto_front.push_back(design);

    test.check(result.pareto_front.front().parameters[0] == 0.25,
               "result.hpp independently exposes owning result records");
    return test.finish();
}
