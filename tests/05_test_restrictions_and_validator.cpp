#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <vector>

int main() {
    test_support::test_context test;
    multobjopt::problem restricted;
    restricted.add_parameter("x", -1.0, 1.0)
        .add_objective("square", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0] * values[0]; })
        .add_restriction("nonnegative", [](multobjopt::scalar_view values) { return values[0]; });

    const auto negative = multobjopt::evaluate_design(restricted, std::vector<double>{-0.25}, 0.0);
    const auto boundary = multobjopt::evaluate_design(restricted, std::vector<double>{0.0}, 0.0);
    const auto positive = multobjopt::evaluate_design(restricted, std::vector<double>{0.25}, 0.0);
    const auto tolerated =
        multobjopt::evaluate_design(restricted, std::vector<double>{-0.25}, 0.25);

    test.check(!negative.feasible, "a negative restriction is a violation");
    test.check_near(negative.total_violation, 0.25, 1.0e-12,
                    "restriction violation magnitude is accumulated");
    test.check(boundary.feasible, "a zero restriction is feasible");
    test.check(positive.feasible, "a positive restriction is feasible");
    test.check(tolerated.feasible, "constraint tolerance can accept a small negative restriction");

    bool validator_called = false;
    multobjopt::problem vetoed;
    vetoed.add_parameter("x", 0.0, 1.0)
        .add_objective("x", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0]; })
        .set_validation(
            [&](multobjopt::scalar_view objectives, multobjopt::scalar_view restrictions) {
                validator_called = objectives.size() == 1 && restrictions.empty();
                return false;
            });
    const auto veto = multobjopt::evaluate_design(vetoed, std::vector<double>{0.5}, 0.0);
    test.check(validator_called, "validator is called even without restrictions");
    test.check(!veto.feasible, "the overall validator can veto a design");
    test.check_near(veto.total_violation, 1.0, 0.0,
                    "a validator veto contributes a deterministic violation");

    return test.finish();
}
