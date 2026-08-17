#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

multobjopt::evaluated_design design(std::vector<double> objectives, bool feasible = true,
                                    double violation = 0.0) {
    multobjopt::evaluated_design result;
    result.objectives = std::move(objectives);
    result.feasible = feasible;
    result.total_violation = violation;
    return result;
}

} // namespace

int main() {
    test_support::test_context test;
    multobjopt::problem problem;
    problem.add_parameter("x", 0.0, 1.0)
        .add_objective("cost", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0]; })
        .add_objective("performance", multobjopt::objective_sense::maximize,
                       [](multobjopt::scalar_view values) { return values[0]; });

    const auto strong = design({1.0, 5.0});
    const auto weak = design({2.0, 4.0});
    const auto tradeoff = design({0.5, 3.0});
    const auto equal = design({1.0, 5.0});

    test.check(multobjopt::dominates(problem, strong, weak),
               "better values in both directions dominate");
    test.check(!multobjopt::dominates(problem, strong, tradeoff),
               "a mixed tradeoff is nondominated");
    test.check(!multobjopt::dominates(problem, tradeoff, strong),
               "nondominance is symmetric for a tradeoff");
    test.check(!multobjopt::dominates(problem, strong, equal),
               "equal objective vectors do not dominate one another");

    const auto feasible = design({100.0, -100.0}, true, 0.0);
    const auto infeasible = design({0.0, 100.0}, false, 0.1);
    test.check(multobjopt::dominates(problem, feasible, infeasible),
               "feasibility takes precedence over objective quality");
    test.check(multobjopt::dominates(problem, design({}, false, 0.1), design({}, false, 0.5)),
               "smaller violation dominates among infeasible designs");

    test.check_throws<std::invalid_argument>(
        [&problem] {
            static_cast<void>(multobjopt::dominates(problem, design({1.0}), design({2.0, 3.0})));
        },
        "feasible objective vector dimensions are checked");

    const auto nonfinite = design({std::numeric_limits<double>::quiet_NaN(), 5.0});
    test.check(!multobjopt::dominates(problem, nonfinite, weak),
               "non-finite objectives never establish dominance");

    return test.finish();
}
