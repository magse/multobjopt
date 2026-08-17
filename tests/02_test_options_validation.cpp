#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <stdexcept>

int main() {
    test_support::test_context test;
    const multobjopt::optimizer_options defaults;
    defaults.validate();

    const auto invalid = [&test, &defaults](auto mutate, const char* message) {
        auto options = defaults;
        mutate(options);
        test.check_throws<std::invalid_argument>([&options] { options.validate(); }, message);
    };

    invalid([](auto& value) { value.max_evaluations = 0; }, "zero evaluation budget is rejected");
    invalid([](auto& value) { value.max_iterations = 0; }, "zero iteration budget is rejected");
    invalid([](auto& value) { value.stall_iterations = 0; }, "zero stall limit is rejected");
    invalid([](auto& value) { value.constraint_tolerance = -1.0; },
            "negative constraint tolerance is rejected");
    invalid([](auto& value) { value.penalty_factor = 0.0; }, "zero penalty factor is rejected");
    invalid([](auto& value) { value.initial_temperature = 0.0; },
            "zero initial temperature is rejected");
    invalid([](auto& value) { value.final_temperature = value.initial_temperature; },
            "final temperature must be below the initial temperature");
    invalid([](auto& value) { value.cooling_rate = 1.0; }, "cooling rate must be below one");
    invalid([](auto& value) { value.population_size = 1; },
            "a genetic population needs at least two members");
    invalid([](auto& value) { value.elite_count = value.population_size + 1; },
            "elite count cannot exceed the population size");
    invalid([](auto& value) { value.crossover_rate = 1.1; },
            "crossover probability cannot exceed one");
    invalid([](auto& value) { value.mutation_rate = -0.1; },
            "mutation probability cannot be negative");
    invalid([](auto& value) { value.complex_size = 1; }, "a Box complex cannot contain one point");
    invalid([](auto& value) { value.contraction_factor = 1.0; },
            "contraction factor must be below one");
    invalid([](auto& value) { value.finite_difference_step = 0.0; },
            "finite-difference step must be positive");
    invalid([](auto& value) { value.line_search_decay = 0.0; },
            "line-search decay must be positive");
    invalid([](auto& value) { value.minimum_step_scale = value.initial_step_scale; },
            "minimum step must be below the initial step");

    return test.finish();
}
