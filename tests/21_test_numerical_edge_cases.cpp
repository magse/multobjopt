#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

int main() {
    test_support::test_context test;

    multobjopt::problem reversed;
    reversed.add_parameter("x", 2.0, 1.0)
        .add_objective("x", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0]; });
    test.check_throws<std::invalid_argument>(
        [&reversed] {
            static_cast<void>(multobjopt::normalize_design(reversed, std::vector<double>{1.5}));
        },
        "public normalization validates bounds before clamping");

    multobjopt::problem three_objectives;
    three_objectives.add_parameter("x", 0.0, 1.0);
    for (int index = 0; index < 3; ++index) {
        three_objectives.add_objective("objective_" + std::to_string(index),
                                       multobjopt::objective_sense::minimize,
                                       [](multobjopt::scalar_view values) { return values[0]; });
    }
    const auto make_design = [](std::vector<double> objectives) {
        multobjopt::evaluated_design design;
        design.objectives = std::move(objectives);
        design.valid = true;
        design.feasible = true;
        design.total_violation = 0.0;
        return design;
    };
    const auto a = make_design({0.0, 0.0, 0.0});
    const auto b = make_design({1.1, -0.6, -0.6});
    const auto c = make_design({0.5, 0.5, -1.2});
    const bool cycle = multobjopt::dominates(three_objectives, a, b, 1.0) &&
                       multobjopt::dominates(three_objectives, b, c, 1.0) &&
                       multobjopt::dominates(three_objectives, c, a, 1.0);
    test.check(!cycle, "dominance tolerance cannot create a cycle");

    multobjopt::problem partly_nonfinite;
    partly_nonfinite.add_parameter("x", -1.0, 1.0)
        .add_objective("sometimes_nan", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) {
                           return values[0] < 0.0 ? std::numeric_limits<double>::quiet_NaN()
                                                  : values[0];
                       });
    multobjopt::optimizer_options nan_options;
    nan_options.algorithm = multobjopt::optimization_algorithm::genetic_algorithm;
    nan_options.initial_guess = {0.5};
    nan_options.population_size = 8;
    nan_options.elite_count = 2;
    nan_options.max_evaluations = 80;
    nan_options.max_iterations = 20;
    const auto nan_result = multobjopt::optimize(partly_nonfinite, nan_options);
    test.check(nan_result.best_design.valid && nan_result.best_design.feasible,
               "non-finite GA individuals sort behind valid individuals");

    multobjopt::problem enormous_grid;
    enormous_grid.add_parameter("x", 0.0, 1.0e20, 1.0)
        .add_objective("distance", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return std::abs(values[0] - 4.0e19); });
    multobjopt::optimizer_options grid_options;
    grid_options.algorithm = multobjopt::optimization_algorithm::genetic_algorithm;
    grid_options.population_size = 6;
    grid_options.elite_count = 1;
    grid_options.max_evaluations = 30;
    const auto grid_result = multobjopt::optimize(enormous_grid, grid_options);
    test.check(grid_result.best_design.valid,
               "a grid larger than an integer mutation type remains valid");
    test.check(grid_result.evaluations <= grid_options.max_evaluations,
               "an enormous grid still respects the budget");

    return test.finish();
}
