#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <cstddef>

namespace {

multobjopt::problem make_problem() {
    multobjopt::problem problem;
    problem.add_parameter("x", -4.0, 4.0, 0.25)
        .add_parameter("y", -4.0, 4.0, 0.25)
        .add_objective("quadratic", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) {
                           const double x_error = values[0] - 1.0;
                           const double y_error = values[1] + 1.5;
                           return x_error * x_error + y_error * y_error;
                       });
    return problem;
}

multobjopt::optimization_result run(multobjopt::optimization_algorithm algorithm) {
    auto problem = make_problem();
    multobjopt::optimizer_options options;
    options.algorithm = algorithm;
    options.max_evaluations = 200;
    options.max_iterations = 20;
    options.stall_iterations = 30;
    options.random_seed = 99'173;
    options.initial_guess = {-3.0, 3.0};
    options.moves_per_temperature = 6;
    options.population_size = 20;
    options.elite_count = 2;
    options.mutation_rate = 0.25;
    return multobjopt::optimize(problem, options);
}

void compare_results(test_support::test_context& test, const multobjopt::optimization_result& first,
                     const multobjopt::optimization_result& second) {
    test.check(first.algorithm_used == second.algorithm_used, "seeded runs report the same method");
    test.check(first.evaluations == second.evaluations,
               "seeded runs use the same evaluation count");
    test.check(first.iterations == second.iterations, "seeded runs use the same iteration count");
    test.check(first.reason == second.reason, "seeded runs have the same termination reason");
    test.check(first.best_design.parameters == second.best_design.parameters,
               "seeded runs return identical parameters");
    test.check(first.best_design.objectives == second.best_design.objectives,
               "seeded runs return identical objective values");
    test.check(first.pareto_front.size() == second.pareto_front.size(),
               "seeded runs return archives of equal size");
    if (first.pareto_front.size() == second.pareto_front.size()) {
        for (std::size_t index = 0; index < first.pareto_front.size(); ++index) {
            test.check(first.pareto_front[index].parameters ==
                           second.pareto_front[index].parameters,
                       "seeded archive parameters are identical");
            test.check(first.pareto_front[index].objectives ==
                           second.pareto_front[index].objectives,
                       "seeded archive objectives are identical");
        }
    }
}

} // namespace

int main() {
    test_support::test_context test;
    const auto annealing_first = run(multobjopt::optimization_algorithm::simulated_annealing);
    const auto annealing_second = run(multobjopt::optimization_algorithm::simulated_annealing);
    compare_results(test, annealing_first, annealing_second);

    const auto genetic_first = run(multobjopt::optimization_algorithm::genetic_algorithm);
    const auto genetic_second = run(multobjopt::optimization_algorithm::genetic_algorithm);
    compare_results(test, genetic_first, genetic_second);

    return test.finish();
}
