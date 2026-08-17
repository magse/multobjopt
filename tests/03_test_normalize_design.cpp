#include "test_support.hpp"

#include <multobjopt/multobjopt.hpp>

#include <limits>
#include <stdexcept>
#include <vector>

int main() {
    test_support::test_context test;
    multobjopt::problem problem;
    problem.add_parameter("continuous", -1.0, 1.0)
        .add_parameter("quantized", -1.0, 5.0, 2.0)
        .add_parameter("unaligned_upper", 0.0, 1.0, 0.3)
        .add_parameter("fixed", 4.0, 4.0)
        .add_objective("unused", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view) { return 0.0; });
    problem.validate();

    const std::vector<double> raw{-8.0, 0.0, 1.0, 99.0};
    const auto normalized = multobjopt::normalize_design(problem, raw);
    test.check(normalized.size() == 4, "normalization preserves dimension");
    test.check_near(normalized[0], -1.0, 0.0, "continuous values are clamped");
    test.check_near(normalized[1], 1.0, 0.0, "half-grid ties round toward the higher grid index");
    test.check_near(normalized[2], 0.9, 1.0e-12,
                    "an unaligned upper bound is not an extra grid point");
    test.check_near(normalized[3], 4.0, 0.0, "fixed parameters remain fixed");

    const std::vector<double> lower_anchor{0.0, -0.1, 0.44, 4.0};
    const auto anchored = multobjopt::normalize_design(problem, lower_anchor);
    test.check_near(anchored[1], -1.0, 0.0, "resolution grids are anchored at the lower bound");
    test.check_near(anchored[2], 0.3, 1.0e-12, "quantization chooses the nearest grid point");

    test.check_throws<std::invalid_argument>(
        [&problem] {
            const std::vector<double> wrong_size{0.0};
            static_cast<void>(multobjopt::normalize_design(problem, wrong_size));
        },
        "a mismatched design dimension is rejected");

    test.check_throws<std::invalid_argument>(
        [&problem] {
            const std::vector<double> nonfinite{std::numeric_limits<double>::quiet_NaN(), 0.0, 0.0,
                                                4.0};
            static_cast<void>(multobjopt::normalize_design(problem, nonfinite));
        },
        "non-finite parameter values are rejected");

    return test.finish();
}
