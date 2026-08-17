#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

int main() {
    test_support::test_context test;

    multobjopt::problem nonfinite_objective;
    nonfinite_objective.add_parameter("x", 0.0, 0.0)
        .add_objective("nan", multobjopt::objective_sense::minimize, [](multobjopt::scalar_view) {
            return std::numeric_limits<double>::quiet_NaN();
        });
    const auto nan_result =
        multobjopt::evaluate_design(nonfinite_objective, std::vector<double>{0.0});
    test.check(!nan_result.feasible, "a NaN objective makes a design infeasible");
    test.check(std::isinf(nan_result.scalarized_objective),
               "a NaN objective produces infinite scalarized cost");

    multobjopt::problem nonfinite_restriction;
    nonfinite_restriction.add_parameter("x", 0.0, 1.0)
        .add_objective("finite", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0]; })
        .add_restriction("infinite", [](multobjopt::scalar_view) {
            return std::numeric_limits<double>::infinity();
        });
    const auto infinite_result =
        multobjopt::evaluate_design(nonfinite_restriction, std::vector<double>{0.5});
    test.check(!infinite_result.feasible, "a non-finite restriction makes a design infeasible");
    test.check(std::isinf(infinite_result.total_violation),
               "a non-finite restriction has infinite violation");

    test.check_throws<std::invalid_argument>(
        [&nonfinite_restriction] {
            static_cast<void>(
                multobjopt::evaluate_design(nonfinite_restriction, std::vector<double>{0.5},
                                            std::numeric_limits<double>::quiet_NaN()));
        },
        "a non-finite constraint tolerance is rejected");

    test.check_throws<std::invalid_argument>(
        [&nonfinite_restriction] {
            static_cast<void>(multobjopt::normalize_design(
                nonfinite_restriction,
                std::vector<double>{std::numeric_limits<double>::infinity()}));
        },
        "a non-finite design parameter is rejected");

    multobjopt::problem throwing_callback;
    throwing_callback.add_parameter("x", 0.0, 1.0)
        .add_objective("throws", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view) -> double {
                           throw std::runtime_error("user callback failed");
                       });
    test.check_throws<std::runtime_error>(
        [&throwing_callback] {
            static_cast<void>(
                multobjopt::evaluate_design(throwing_callback, std::vector<double>{0.5}));
        },
        "exceptions from user callbacks propagate to the caller");

    multobjopt::optimizer_options options;
    options.algorithm = multobjopt::optimization_algorithm::gradient_descent;
    const auto optimized_nan = multobjopt::optimize(nonfinite_objective, options);
    test.check(!optimized_nan.best_design.feasible,
               "optimization reports an all-nonfinite fixed design as infeasible");
    test.check(optimized_nan.pareto_front.empty(),
               "non-finite designs are excluded from the Pareto archive");

    return test.finish();
}
