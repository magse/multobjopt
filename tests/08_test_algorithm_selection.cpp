#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <string>

namespace {

void add_objective(multobjopt::problem& problem, const std::string& name = "value") {
    problem.add_objective(name, multobjopt::objective_sense::minimize,
                          [](multobjopt::scalar_view values) { return values.front(); });
}

} // namespace

int main() {
    test_support::test_context test;

    multobjopt::problem small_continuous;
    small_continuous.add_parameter("x", 0.0, 1.0);
    add_objective(small_continuous);
    test.check(multobjopt::select_algorithm(small_continuous) ==
                   multobjopt::optimization_algorithm::gradient_descent,
               "small unconstrained continuous problems select gradient descent");

    multobjopt::problem large_continuous;
    for (int index = 0; index < 13; ++index) {
        large_continuous.add_parameter("x" + std::to_string(index), 0.0, 1.0);
    }
    add_objective(large_continuous);
    test.check(multobjopt::select_algorithm(large_continuous) ==
                   multobjopt::optimization_algorithm::simulated_annealing,
               "larger unconstrained continuous problems select annealing");

    multobjopt::problem quantized;
    quantized.add_parameter("x", 0.0, 1.0, 0.1);
    add_objective(quantized);
    test.check(multobjopt::select_algorithm(quantized) ==
                   multobjopt::optimization_algorithm::genetic_algorithm,
               "quantized problems select the genetic algorithm");

    auto multiobjective = small_continuous;
    add_objective(multiobjective, "second");
    test.check(multobjopt::select_algorithm(multiobjective) ==
                   multobjopt::optimization_algorithm::genetic_algorithm,
               "multi-objective problems select the genetic algorithm");

    auto constrained = small_continuous;
    constrained.add_restriction("margin",
                                [](multobjopt::scalar_view values) { return values.front(); });
    test.check(multobjopt::select_algorithm(constrained) ==
                   multobjopt::optimization_algorithm::box_method,
               "continuous constrained problems select the Box method");

    auto validated = small_continuous;
    validated.set_validation([](multobjopt::scalar_view, multobjopt::scalar_view) { return true; });
    test.check(multobjopt::select_algorithm(validated) ==
                   multobjopt::optimization_algorithm::box_method,
               "an overall validator makes a continuous problem constrained");

    auto constrained_multiobjective = constrained;
    add_objective(constrained_multiobjective, "second");
    test.check(multobjopt::select_algorithm(constrained_multiobjective) ==
                   multobjopt::optimization_algorithm::genetic_algorithm,
               "multi-objective selection takes precedence over constraints");

    return test.finish();
}
