#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <vector>

int main() {
    test_support::test_context test;

    multobjopt::problem mixed;
    mixed.add_parameter("x", 0.0, 4.0)
        .add_objective(
            "cost", multobjopt::objective_sense::minimize,
            [](multobjopt::scalar_view values) { return values[0]; }, 2.0)
        .add_objective(
            "performance", multobjopt::objective_sense::maximize,
            [](multobjopt::scalar_view values) { return 2.0 * values[0]; }, 0.5);

    const auto mixed_value = multobjopt::evaluate_design(mixed, std::vector<double>{3.0});
    test.check(mixed_value.objectives == std::vector<double>({3.0, 6.0}),
               "objective values remain in user-facing directions");
    test.check_near(mixed_value.scalarized_objective, 3.0, 1.0e-12,
                    "maximization objectives are negated only for scalarization");

    multobjopt::problem maximize;
    maximize.add_parameter("x", 0.0, 4.0)
        .add_objective("performance", multobjopt::objective_sense::maximize,
                       [](multobjopt::scalar_view values) { return values[0]; });
    const auto high = multobjopt::evaluate_design(maximize, std::vector<double>{3.0});
    const auto low = multobjopt::evaluate_design(maximize, std::vector<double>{1.0});
    test.check(multobjopt::dominates(maximize, high, low),
               "a larger maximization value dominates a smaller one");
    test.check(!multobjopt::dominates(maximize, low, high),
               "a smaller maximization value does not dominate a larger one");

    multobjopt::problem minimize;
    minimize.add_parameter("x", 0.0, 4.0)
        .add_objective("cost", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0]; });
    const auto cheap = multobjopt::evaluate_design(minimize, std::vector<double>{1.0});
    const auto expensive = multobjopt::evaluate_design(minimize, std::vector<double>{3.0});
    test.check(multobjopt::dominates(minimize, cheap, expensive),
               "a smaller minimization value dominates a larger one");

    return test.finish();
}
