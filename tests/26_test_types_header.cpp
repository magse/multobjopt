#include <multobjopt/types.hpp>

#include "test_support.hpp"

#include <string>

int main() {
    test_support::test_context test;

    multobjopt::parameter_definition parameter{"length", 0.0, 2.0, 0.1};
    multobjopt::objective_definition objective{
        "mass",
        multobjopt::objective_sense::minimize,
        [](multobjopt::scalar_view values) { return values.front(); },
        1.0,
    };
    multobjopt::restriction_definition restriction{
        "margin",
        [](multobjopt::scalar_view values) { return values.front(); },
        1.0,
    };

    test.check(parameter.name == "length", "types.hpp defines parameter metadata");
    test.check(objective.function(multobjopt::scalar_view{&parameter.upper_bound, 1}) == 2.0,
               "types.hpp defines scalar callbacks");
    test.check(restriction.scale == 1.0, "types.hpp defines restriction scaling");
    return test.finish();
}
