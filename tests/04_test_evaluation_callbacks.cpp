#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <vector>

int main() {
    test_support::test_context test;
    std::vector<int> call_order;
    bool callback_parameters_are_correct = true;
    bool validator_values_are_correct = false;

    const auto inspect_parameters = [&](multobjopt::scalar_view values) {
        callback_parameters_are_correct = callback_parameters_are_correct && values.size() == 2 &&
                                          values[0] == 2.0 && values[1] == -1.0;
    };

    multobjopt::problem problem;
    problem.add_parameter("x", 0.0, 3.0)
        .add_parameter("y", -2.0, 2.0)
        .add_objective(
            "sum", multobjopt::objective_sense::minimize,
            [&](multobjopt::scalar_view values) {
                call_order.push_back(1);
                inspect_parameters(values);
                return values[0] + values[1];
            },
            2.0)
        .add_objective(
            "difference", multobjopt::objective_sense::maximize,
            [&](multobjopt::scalar_view values) {
                call_order.push_back(2);
                inspect_parameters(values);
                return values[0] - values[1];
            },
            0.5)
        .add_restriction("x_margin",
                         [&](multobjopt::scalar_view values) {
                             call_order.push_back(3);
                             inspect_parameters(values);
                             return values[0];
                         })
        .add_restriction("y_margin",
                         [&](multobjopt::scalar_view values) {
                             call_order.push_back(4);
                             inspect_parameters(values);
                             return values[1] + 2.0;
                         })
        .set_validation(
            [&](multobjopt::scalar_view objectives, multobjopt::scalar_view restrictions) {
                call_order.push_back(5);
                validator_values_are_correct = objectives.size() == 2 && restrictions.size() == 2 &&
                                               objectives[0] == 1.0 && objectives[1] == 3.0 &&
                                               restrictions[0] == 2.0 && restrictions[1] == 1.0;
                return true;
            });

    const std::vector<double> parameters{2.0, -1.0};
    const auto result = multobjopt::evaluate_design(problem, parameters, 0.0);

    test.check(callback_parameters_are_correct,
               "all scalar callbacks receive the normalized design");
    test.check(validator_values_are_correct,
               "the validator receives all objective and restriction values");
    test.check(call_order == std::vector<int>({1, 2, 3, 4, 5}),
               "callbacks run once each in definition order with validation last");
    test.check(result.parameters == parameters, "evaluated parameters are retained");
    test.check(result.objectives == std::vector<double>({1.0, 3.0}),
               "raw objective values are retained");
    test.check(result.restrictions == std::vector<double>({2.0, 1.0}),
               "raw restriction values are retained");
    test.check_near(result.scalarized_objective, 0.5, 1.0e-12,
                    "weights and objective senses are applied during scalarization");
    test.check(result.feasible, "accepted finite values form a feasible design");

    return test.finish();
}
