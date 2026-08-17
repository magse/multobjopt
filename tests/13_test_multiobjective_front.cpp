#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <cmath>
#include <cstddef>

int main() {
    test_support::test_context test;
    multobjopt::problem problem;
    problem.add_parameter("tradeoff", 0.0, 4.0, 1.0)
        .add_objective("cost", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0]; })
        .add_objective("performance", multobjopt::objective_sense::maximize,
                       [](multobjopt::scalar_view values) { return values[0]; });

    multobjopt::optimizer_options options;
    options.algorithm = multobjopt::optimization_algorithm::automatic;
    options.max_evaluations = 240;
    options.max_iterations = 8;
    options.stall_iterations = 12;
    options.random_seed = 83'219;
    options.population_size = 48;
    options.elite_count = 2;
    options.mutation_rate = 0.25;

    const auto result = multobjopt::optimize(problem, options);
    test.check(result.algorithm_used == multobjopt::optimization_algorithm::genetic_algorithm,
               "automatic selection uses the genetic algorithm for multiple objectives");
    test.check(result.pareto_front.size() >= 3,
               "the run discovers several points on the tradeoff front");

    for (std::size_t index = 0; index < result.pareto_front.size(); ++index) {
        const auto& design = result.pareto_front[index];
        test.check(design.feasible, "the Pareto archive contains only feasible designs");
        test.check(design.parameters.size() == 1 && design.objectives.size() == 2,
                   "archived designs retain parameters and both objectives");
        test.check_near(design.objectives[0], design.parameters[0], 0.0,
                        "the cost objective is retained in the archive");
        test.check_near(design.objectives[1], design.parameters[0], 0.0,
                        "the performance objective is retained in the archive");
        test.check_near(design.parameters[0], std::round(design.parameters[0]), 1.0e-12,
                        "archived designs respect parameter resolution");
        if (index > 0) {
            test.check(result.pareto_front[index - 1].objectives[0] < design.objectives[0],
                       "the archive is deterministic, sorted, and free of duplicates");
        }
        for (std::size_t other = 0; other < result.pareto_front.size(); ++other) {
            if (index != other) {
                test.check(!multobjopt::dominates(problem, result.pareto_front[other], design),
                           "no archived point is dominated by another archived point");
            }
        }
    }

    return test.finish();
}
