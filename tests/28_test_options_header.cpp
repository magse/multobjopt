#include <multobjopt/options.hpp>

#include "test_support.hpp"

int main() {
    test_support::test_context test;

    multobjopt::optimizer_options options;
    options.algorithm = multobjopt::optimization_algorithm::simulated_annealing;
    options.validate();

    test.check(options.max_evaluations > 0,
               "options.hpp independently exposes validated solver controls");
    return test.finish();
}
