#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <array>
#include <cstddef>

namespace {

multobjopt::optimization_result run_with_budget(multobjopt::optimization_algorithm algorithm,
                                                std::size_t& callback_count) {
    multobjopt::problem problem;
    problem.add_parameter("x", -1.0, 1.0)
        .add_objective("slope", multobjopt::objective_sense::minimize,
                       [&callback_count](multobjopt::scalar_view values) {
                           ++callback_count;
                           return -values[0];
                       });

    multobjopt::optimizer_options options;
    options.algorithm = algorithm;
    options.max_evaluations = 5;
    options.max_iterations = 100;
    options.stall_iterations = 100;
    options.random_seed = 4'041;
    options.initial_guess = {-1.0};
    options.moves_per_temperature = 3;
    options.population_size = 4;
    options.elite_count = 1;
    options.complex_size = 3;
    return multobjopt::optimize(problem, options);
}

} // namespace

int main() {
    test_support::test_context test;
    constexpr std::array algorithms{
        multobjopt::optimization_algorithm::simulated_annealing,
        multobjopt::optimization_algorithm::genetic_algorithm,
        multobjopt::optimization_algorithm::box_method,
        multobjopt::optimization_algorithm::gradient_descent,
    };

    for (const auto algorithm : algorithms) {
        std::size_t callback_count = 0;
        const auto result = run_with_budget(algorithm, callback_count);
        test.check(result.algorithm_used == algorithm,
                   "the explicit algorithm is retained at a small budget");
        test.check(result.evaluations == 5,
                   "each method consumes but never exceeds the five-evaluation budget");
        test.check(callback_count == result.evaluations,
                   "reported evaluations equal objective callback invocations");
        test.check(result.reason == multobjopt::termination_reason::evaluation_limit,
                   "budget exhaustion reports evaluation_limit");
    }

    return test.finish();
}
