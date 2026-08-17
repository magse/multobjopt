#include <multobjopt/evaluation.hpp>
#include <multobjopt/version.hpp>

#include <vector>

int main() {
    multobjopt::problem problem;
    problem.add_parameter("x", 0.0, 1.0)
        .add_objective("x", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0]; });

    const auto result = multobjopt::evaluate_design(problem, std::vector<double>{0.25});
    return result.valid && result.feasible && result.objectives[0] == 0.25 &&
                   !multobjopt::version::library_version.empty()
               ? 0
               : 1;
}
