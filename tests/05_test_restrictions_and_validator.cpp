#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <vector>

/**
 * @file 05_test_restrictions_and_validator.cpp
 * @brief Executable specification for scalar margins and Boolean validation.
 *
 * Restrictions expose a distance from their zero boundary; the overall
 * validator exposes only a final yes/no decision. The two mechanisms therefore
 * have deliberately different violation accounting.
 */

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

    // A final validator may veto an otherwise finite design even when no scalar
    // restrictions exist. It sees objective results, not parameter values.
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
    test.check(veto.valid, "a Boolean veto does not make finite model outputs invalid");
    test.check(!veto.feasible, "the overall validator can veto a design");
    test.check_near(veto.total_violation, 1.0, 0.0,
                    "a validator veto contributes a deterministic violation");

    return test.finish();
}
