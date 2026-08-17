#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <array>
#include <cstddef>

namespace {

multobjopt::optimization_result run(multobjopt::optimization_algorithm algorithm,
                                    std::size_t budget, std::size_t& calls) {
    multobjopt::problem problem;
    problem.add_parameter("x", -1.0, 1.0)
        .add_objective("x", multobjopt::objective_sense::minimize,
                       [&calls](multobjopt::scalar_view values) {
                           ++calls;
                           return values[0] * values[0];
                       });
    multobjopt::optimizer_options options;
    options.algorithm = algorithm;
    options.max_evaluations = budget;
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
        for (const std::size_t budget : {std::size_t{1}, std::size_t{2}}) {
            std::size_t calls = 0;
            const auto result = run(algorithm, budget, calls);
            test.check(result.evaluations == budget, "tiny budgets are consumed exactly");
            test.check(calls == budget, "tiny-budget callback accounting is exact");
            test.check(result.reason == multobjopt::termination_reason::evaluation_limit,
                       "exhausted initialization reports the evaluation limit");
        }
    }

    for (const auto algorithm : algorithms) {
        std::size_t calls = 0;
        multobjopt::problem fixed;
        fixed.add_parameter("x", 0.25, 0.25)
            .add_objective("x", multobjopt::objective_sense::minimize,
                           [&calls](multobjopt::scalar_view values) {
                               ++calls;
                               return values[0];
                           });
        multobjopt::optimizer_options options;
        options.algorithm = algorithm;
        const auto result = multobjopt::optimize(fixed, options);
        test.check(calls == 1 && result.evaluations == 1,
                   "a fixed design is evaluated exactly once");
        test.check(result.reason == multobjopt::termination_reason::converged,
                   "a valid fixed design converges immediately");
    }

    return test.finish();
}
