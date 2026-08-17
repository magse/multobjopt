#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <limits>
#include <stdexcept>
#include <string>

namespace {

multobjopt::scalar objective(multobjopt::scalar_view values) {
    return values.front() * values.front();
}

multobjopt::problem basic_problem() {
    multobjopt::problem result;
    result.add_parameter("x", -2.0, 2.0)
        .add_objective("square", multobjopt::objective_sense::minimize, objective);
    return result;
}

} // namespace

int main() {
    test_support::test_context test;

    auto valid = basic_problem();
    valid.add_parameter("fixed", 3.0, 3.0)
        .add_restriction("finite_margin", [](multobjopt::scalar_view) { return 1.0; });
    valid.validate();
    test.check(valid.parameters().size() == 2, "parameters retain insertion order");
    test.check(valid.parameters()[1].name == "fixed", "parameter names are retained");
    test.check(valid.objectives().size() == 1, "objective is registered");
    test.check(valid.restrictions().size() == 1, "restriction is registered");

    test.check_throws<std::invalid_argument>(
        [] {
            multobjopt::problem empty;
            empty.validate();
        },
        "a problem without parameters is rejected");

    test.check_throws<std::invalid_argument>(
        [] {
            multobjopt::problem no_objective;
            no_objective.add_parameter("x", 0.0, 1.0);
            no_objective.validate();
        },
        "a problem without objectives is rejected");

    test.check_throws<std::invalid_argument>(
        [] {
            auto duplicate = basic_problem();
            duplicate.add_parameter("x", 0.0, 1.0);
            duplicate.validate();
        },
        "duplicate parameter names are rejected");

    test.check_throws<std::invalid_argument>(
        [] {
            auto reversed = basic_problem();
            reversed.add_parameter("bad", 2.0, -2.0);
            reversed.validate();
        },
        "reversed bounds are rejected");

    test.check_throws<std::invalid_argument>(
        [] {
            auto nonfinite = basic_problem();
            nonfinite.add_parameter("bad", 0.0, std::numeric_limits<double>::infinity());
            nonfinite.validate();
        },
        "non-finite bounds are rejected");

    test.check_throws<std::invalid_argument>(
        [] {
            auto negative_resolution = basic_problem();
            negative_resolution.add_parameter("bad", 0.0, 1.0, -0.1);
            negative_resolution.validate();
        },
        "negative resolution is rejected");

    test.check_throws<std::invalid_argument>(
        [] {
            auto oversized_resolution = basic_problem();
            oversized_resolution.add_parameter("bad", 0.0, 1.0, 2.0);
            oversized_resolution.validate();
        },
        "resolution larger than the range is rejected");

    test.check_throws<std::invalid_argument>(
        [] {
            auto duplicate = basic_problem();
            duplicate.add_objective("square", multobjopt::objective_sense::maximize, objective);
            duplicate.validate();
        },
        "duplicate objective names are rejected");

    test.check_throws<std::invalid_argument>(
        [] {
            auto empty_function = basic_problem();
            empty_function.add_restriction("missing", {});
            empty_function.validate();
        },
        "empty restriction callbacks are rejected");

    test.check_throws<std::invalid_argument>(
        [] {
            auto too_many = basic_problem();
            for (int index = 1; index < 26; ++index) {
                too_many.add_parameter("x" + std::to_string(index), 0.0, 1.0);
            }
            too_many.validate();
        },
        "more than twenty-five parameters are rejected");

    return test.finish();
}
