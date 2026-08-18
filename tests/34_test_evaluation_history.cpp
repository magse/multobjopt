#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <array>
#include <cstddef>
#include <vector>

namespace {

struct callback_observations {
    std::vector<multobjopt::scalar> parameters;
    std::size_t objective_calls{0};
    std::size_t restriction_calls{0};
    std::size_t validation_calls{0};
};

multobjopt::optimization_result run_with_history(multobjopt::optimization_algorithm algorithm,
                                                 bool record_evaluation_history,
                                                 callback_observations& observations) {
    multobjopt::problem problem;
    problem.add_parameter("x", -1.0, 1.0, 0.25)
        .add_objective("square", multobjopt::objective_sense::minimize,
                       [&observations](multobjopt::scalar_view values) {
                           observations.parameters.push_back(values[0]);
                           ++observations.objective_calls;
                           return values[0] * values[0];
                       })
        .add_restriction("lower_margin",
                         [&observations](multobjopt::scalar_view values) {
                             ++observations.restriction_calls;
                             return values[0] + 0.5;
                         })
        .set_validation([&observations](multobjopt::scalar_view objectives,
                                        multobjopt::scalar_view restrictions) {
            ++observations.validation_calls;
            return objectives[0] <= 0.81 && restrictions.size() == 1;
        });

    multobjopt::optimizer_options options;
    options.algorithm = algorithm;
    options.max_evaluations = 5;
    options.max_iterations = 100;
    options.stall_iterations = 100;
    options.random_seed = 34'034;
    options.initial_guess = {-1.0};
    options.moves_per_temperature = 3;
    options.population_size = 4;
    options.elite_count = 1;
    options.complex_size = 3;
    options.record_evaluation_history = record_evaluation_history;
    return multobjopt::optimize(problem, options);
}

} // namespace

int main() {
    test_support::test_context test;
    const multobjopt::optimizer_options default_options;
    test.check(!default_options.record_evaluation_history,
               "evaluation-history recording is disabled by default");

    constexpr std::array algorithms{
        multobjopt::optimization_algorithm::simulated_annealing,
        multobjopt::optimization_algorithm::genetic_algorithm,
        multobjopt::optimization_algorithm::box_method,
        multobjopt::optimization_algorithm::gradient_descent,
    };

    for (const auto algorithm : algorithms) {
        callback_observations observations;
        const auto result = run_with_history(algorithm, true, observations);

        test.check(result.evaluations == 5,
                   "every algorithm retains the exact complete-evaluation budget");
        test.check(result.evaluation_history.size() == result.evaluations,
                   "enabled history contains every counted evaluation exactly once");
        test.check(observations.parameters.size() == result.evaluations,
                   "one observed normalized parameter corresponds to each history entry");
        test.check(observations.objective_calls == result.evaluations,
                   "history accounting matches objective callback calls");
        test.check(observations.restriction_calls == result.evaluations,
                   "history accounting matches restriction callback calls");
        test.check(observations.validation_calls == result.evaluations,
                   "history accounting matches overall validation calls");

        for (std::size_t index = 0; index < result.evaluation_history.size(); ++index) {
            const auto& evaluation = result.evaluation_history[index];
            const auto observed_parameter = observations.parameters[index];
            test.check(evaluation.parameters.size() == 1,
                       "history owns one normalized parameter per evaluation");
            test.check(evaluation.objectives.size() == 1,
                       "history owns one raw objective per evaluation");
            test.check(evaluation.restrictions.size() == 1,
                       "history owns one raw restriction per evaluation");
            test.check_near(evaluation.parameters[0], observed_parameter, 0.0,
                            "history order is the exact objective callback order");
            test.check_near(evaluation.objectives[0], observed_parameter * observed_parameter, 0.0,
                            "history keeps the objective from the same evaluation");
            test.check_near(evaluation.restrictions[0], observed_parameter + 0.5, 0.0,
                            "history keeps the restriction from the same evaluation");
        }

        test.check_near(result.evaluation_history.front().parameters[0], -1.0, 0.0,
                        "evaluation one is the normalized configured initial guess");
        test.check(!result.evaluation_history.front().feasible,
                   "history retains designs rejected by restrictions and validation");
    }

    callback_observations unrecorded_observations;
    const auto unrecorded = run_with_history(
        multobjopt::optimization_algorithm::simulated_annealing, false, unrecorded_observations);
    test.check(unrecorded.evaluations == unrecorded_observations.objective_calls,
               "default-style disabled recording does not change evaluation accounting");
    test.check(unrecorded.evaluation_history.empty(),
               "evaluation history stays empty when optional recording is disabled");

    return test.finish();
}
