#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <array>

int main() {
    test_support::test_context test;
    multobjopt::problem problem;
    problem.add_parameter("x", 2.0, 2.0)
        .add_parameter("y", -3.0, -3.0)
        .add_objective("fixed_value", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) {
                           return values[0] * values[0] + values[1] * values[1];
                       })
        .add_restriction("always_valid", [](multobjopt::scalar_view) { return 0.0; });

    constexpr std::array algorithms{
        multobjopt::optimization_algorithm::simulated_annealing,
        multobjopt::optimization_algorithm::genetic_algorithm,
        multobjopt::optimization_algorithm::box_method,
        multobjopt::optimization_algorithm::gradient_descent,
    };
    for (const auto algorithm : algorithms) {
        multobjopt::optimizer_options options;
        options.algorithm = algorithm;
        options.max_evaluations = 100;
        options.max_iterations = 10;
        options.population_size = 8;
        options.elite_count = 2;
        options.complex_size = 4;
        const auto result = multobjopt::optimize(problem, options);

        test.check(result.best_design.parameters == std::vector<double>({2.0, -3.0}),
                   "every method preserves fixed parameter values");
        test.check_near(result.best_design.objectives[0], 13.0, 0.0,
                        "fixed designs are evaluated correctly");
        test.check(result.best_design.feasible,
                   "a fixed design satisfying its restriction is feasible");
        test.check(result.reason == multobjopt::termination_reason::converged,
                   "a fully fixed problem converges immediately");
        test.check(result.pareto_front.size() == 1,
                   "duplicate fixed evaluations collapse to one archive entry");
    }

    return test.finish();
}
