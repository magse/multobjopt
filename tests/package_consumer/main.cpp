#include <multobjopt/evaluation.hpp>
#include <multobjopt/reporting.hpp>
#include <multobjopt/version.hpp>

#include <sstream>
#include <string>
#include <vector>

int main() {
    multobjopt::problem problem;
    problem.add_parameter("x", 0.0, 1.0)
        .add_objective("x", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view values) { return values[0]; });

    const auto design = multobjopt::evaluate_design(problem, std::vector<double>{0.25});

    // Exercise a linked reporting symbol through the installed focused header,
    // not only the source-tree umbrella API.
    multobjopt::optimization_result result;
    result.algorithm_used = multobjopt::optimization_algorithm::gradient_descent;
    result.best_design = design;
    result.pareto_front = {design};
    result.evaluations = 1;

    std::ostringstream summary;
    multobjopt::write_summary_json(summary, problem, result);

    return design.valid && design.feasible && design.objectives[0] == 0.25 &&
                   summary.str().find("multobjopt.optimization_summary") != std::string::npos &&
                   !multobjopt::version::library_version.empty()
               ? 0
               : 1;
}
