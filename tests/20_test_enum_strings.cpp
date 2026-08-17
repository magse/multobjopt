#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <string_view>

int main() {
    test_support::test_context test;
    using multobjopt::optimization_algorithm;
    using multobjopt::termination_reason;

    test.check(multobjopt::to_string(optimization_algorithm::automatic) == "automatic",
               "automatic has a stable snake_case name");
    test.check(multobjopt::to_string(optimization_algorithm::simulated_annealing) ==
                   "simulated_annealing",
               "simulated annealing has a stable snake_case name");
    test.check(multobjopt::to_string(optimization_algorithm::genetic_algorithm) ==
                   "genetic_algorithm",
               "the genetic algorithm has a stable snake_case name");
    test.check(multobjopt::to_string(optimization_algorithm::box_method) == "box_method",
               "the Box method has a stable snake_case name");
    test.check(multobjopt::to_string(optimization_algorithm::gradient_descent) ==
                   "gradient_descent",
               "gradient descent has a stable snake_case name");

    test.check(multobjopt::to_string(termination_reason::evaluation_limit) == "evaluation_limit",
               "evaluation_limit has a stable snake_case name");
    test.check(multobjopt::to_string(termination_reason::iteration_limit) == "iteration_limit",
               "iteration_limit has a stable snake_case name");
    test.check(multobjopt::to_string(termination_reason::stalled) == "stalled",
               "stalled has a stable snake_case name");
    test.check(multobjopt::to_string(termination_reason::converged) == "converged",
               "converged has a stable snake_case name");

    test.check(multobjopt::to_string(static_cast<optimization_algorithm>(999)) == "unknown",
               "unknown algorithm values have a safe fallback name");
    test.check(multobjopt::to_string(static_cast<termination_reason>(999)) == "unknown",
               "unknown termination values have a safe fallback name");

    return test.finish();
}
