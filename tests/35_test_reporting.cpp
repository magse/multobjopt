#include "test_support.hpp"

#include <multobjopt/reporting.hpp>
#include <multobjopt/version.hpp>

#include <cmath>
#include <ios>
#include <limits>
#include <locale>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

/** A stream buffer that lets the writer start but refuses every byte. */
class failing_stream_buffer : public std::streambuf {
  protected:
    std::streamsize xsputn(const char*, std::streamsize) override { return 0; }

    int_type overflow(int_type) override { return traits_type::eof(); }
};

/** Locale punctuation deliberately incompatible with the report schema. */
class comma_decimal_punctuation : public std::numpunct<char> {
  protected:
    char do_decimal_point() const override { return ','; }
    char do_thousands_sep() const override { return '_'; }
    std::string do_grouping() const override { return "\3"; }
};

multobjopt::problem make_problem(std::size_t& callback_calls) {
    multobjopt::problem problem_definition;
    problem_definition.add_parameter("x,axis", 0.0, 1.0)
        .add_objective("cost\"raw", multobjopt::objective_sense::minimize,
                       [&callback_calls](multobjopt::scalar_view parameters) {
                           ++callback_calls;
                           return parameters[0];
                       })
        .add_restriction("margin\tline", [&callback_calls](multobjopt::scalar_view parameters) {
            ++callback_calls;
            return 1.0 - parameters[0];
        });
    return problem_definition;
}

multobjopt::evaluated_design feasible_design() {
    multobjopt::evaluated_design design;
    design.parameters = {0.25};
    design.objectives = {1.5};
    design.restrictions = {0.0};
    design.valid = true;
    design.feasible = true;
    design.total_violation = 0.0;
    design.scalarized_objective = 1.5;
    return design;
}

multobjopt::evaluated_design invalid_design() {
    multobjopt::evaluated_design design;
    design.parameters = {0.5};
    design.objectives = {std::numeric_limits<multobjopt::scalar>::quiet_NaN()};
    design.restrictions = {-std::numeric_limits<multobjopt::scalar>::infinity()};
    design.valid = false;
    design.feasible = false;
    design.total_violation = std::numeric_limits<multobjopt::scalar>::infinity();
    design.scalarized_objective = std::numeric_limits<multobjopt::scalar>::infinity();
    return design;
}

multobjopt::optimization_result make_complete_result() {
    multobjopt::optimization_result result;
    result.algorithm_used = multobjopt::optimization_algorithm::genetic_algorithm;
    result.best_design = feasible_design();
    result.pareto_front = {feasible_design()};
    result.evaluations = 2;
    result.iterations = 1;
    result.reason = multobjopt::termination_reason::evaluation_limit;
    result.evaluation_history = {feasible_design(), invalid_design()};
    return result;
}

multobjopt::optimization_result make_invalid_summary_result() {
    multobjopt::optimization_result result;
    result.algorithm_used = multobjopt::optimization_algorithm::box_method;
    result.best_design = invalid_design();
    result.evaluations = 1;
    result.iterations = 7;
    result.reason = multobjopt::termination_reason::no_feasible_design;
    result.evaluation_history = {invalid_design()};
    return result;
}

std::string expected_invalid_json() {
    return std::string{"{\n"
                       "  \"schema\": \"multobjopt.optimization_summary\",\n"
                       "  \"schema_version\": 1,\n"
                       "  \"library_version\": \""} +
           std::string{multobjopt::version::library_version} +
           "\",\n"
           "  \"algorithm_used\": \"box_method\",\n"
           "  \"termination_reason\": \"no_feasible_design\",\n"
           "  \"evaluations\": 1,\n"
           "  \"iterations\": 7,\n"
           "  \"evaluation_history_recorded\": true,\n"
           "  \"evaluation_history_count\": 1,\n"
           "  \"pareto_front_count\": 0,\n"
           "  \"best_design\": {\n"
           "    \"parameters\": {\n"
           "      \"x,axis\": 0.5\n"
           "    },\n"
           "    \"objectives\": {\n"
           "      \"cost\\\"raw\": null\n"
           "    },\n"
           "    \"restrictions\": {\n"
           "      \"margin\\tline\": null\n"
           "    },\n"
           "    \"valid\": false,\n"
           "    \"feasible\": false,\n"
           "    \"total_violation\": null,\n"
           "    \"scalarized_objective\": null\n"
           "  }\n"
           "}\n";
}

} // namespace

int main() {
    test_support::test_context test;
    std::size_t callback_calls = 0;
    const auto problem_definition = make_problem(callback_calls);
    const auto result = make_complete_result();

    std::ostringstream csv;
    multobjopt::write_evaluation_history(csv, problem_definition, result);
    const std::string expected_csv =
        "evaluation,\"parameter.x,axis\",\"objective.cost\"\"raw\","
        "restriction.margin\tline,valid,feasible,total_violation,scalarized_objective\n"
        "1,0.25,1.5,0,true,true,0,1.5\n"
        "2,0.5,nan,-inf,false,false,inf,inf\n";
    test.check(csv.str() == expected_csv,
               "CSV history has exact prefixed headers, escaping, order, and scalar tokens");

    std::ostringstream localized_csv;
    localized_csv.imbue(std::locale{std::locale::classic(), new comma_decimal_punctuation});
    multobjopt::write_evaluation_history(localized_csv, problem_definition, result);
    test.check(localized_csv.str() == expected_csv,
               "tabular scalars and indices ignore the destination stream locale");
    test.check(std::use_facet<std::numpunct<char>>(localized_csv.getloc()).decimal_point() == ',',
               "reporting leaves the caller-owned stream locale unchanged");

    auto round_trip_result = result;
    round_trip_result.evaluation_history[0].objectives[0] = std::nextafter(1.0, 2.0);
    std::ostringstream round_trip_csv;
    multobjopt::write_evaluation_history(round_trip_csv, problem_definition, round_trip_result);
    test.check(round_trip_csv.str().find(",1.0000000000000002,") != std::string::npos,
               "finite scalars retain enough digits for an exact double round trip");

    std::ostringstream tsv;
    multobjopt::write_pareto_front(tsv, problem_definition, result,
                                   multobjopt::delimited_text_format::tsv);
    const std::string expected_tsv =
        "pareto_index\tparameter.x,axis\t\"objective.cost\"\"raw\"\t"
        "\"restriction.margin\tline\"\tvalid\tfeasible\ttotal_violation\t"
        "scalarized_objective\n"
        "1\t0.25\t1.5\t0\ttrue\ttrue\t0\t1.5\n";
    test.check(tsv.str() == expected_tsv,
               "TSV Pareto output uses tabs, exact names, escaping, and one-based indices");

    const auto invalid_summary = make_invalid_summary_result();
    std::ostringstream json;
    multobjopt::write_summary_json(json, problem_definition, invalid_summary);
    test.check(json.str() == expected_invalid_json(),
               "JSON summary has the stable schema, named design, escaping, and nulls");

    multobjopt::problem control_name_problem;
    std::string control_name{"path\\drive"};
    control_name.push_back('\x01');
    control_name_problem.add_parameter(control_name, 0.0, 1.0)
        .add_objective("cost\"raw", multobjopt::objective_sense::minimize,
                       [](multobjopt::scalar_view parameters) { return parameters[0]; })
        .add_restriction("margin\tline",
                         [](multobjopt::scalar_view parameters) { return 1.0 - parameters[0]; });
    std::ostringstream escaped_json;
    multobjopt::write_summary_json(escaped_json, control_name_problem, invalid_summary);
    test.check(escaped_json.str().find("path\\\\drive\\u0001") != std::string::npos &&
                   escaped_json.str().find("cost\\\"raw") != std::string::npos &&
                   escaped_json.str().find("margin\\tline") != std::string::npos,
               "JSON escapes backslashes, control bytes, quotes, and tabs");

    std::ostringstream empty_pareto;
    multobjopt::write_pareto_front(empty_pareto, problem_definition, invalid_summary);
    const std::string expected_empty_pareto =
        "pareto_index,\"parameter.x,axis\",\"objective.cost\"\"raw\","
        "restriction.margin\tline,valid,feasible,total_violation,scalarized_objective\n";
    test.check(empty_pareto.str() == expected_empty_pareto,
               "an empty feasible Pareto archive produces a useful header-only table");

    std::ostringstream repeated_json;
    multobjopt::write_summary_json(repeated_json, problem_definition, invalid_summary);
    test.check(repeated_json.str() == json.str(),
               "summary serialization is deterministic for the same result");
    test.check(callback_calls == 0, "report writers never invoke user model callbacks");

    auto missing_history = result;
    missing_history.evaluation_history.clear();
    std::ostringstream missing_history_output;
    test.check_throws<std::invalid_argument>(
        [&] {
            multobjopt::write_evaluation_history(missing_history_output, problem_definition,
                                                 missing_history);
        },
        "history writer rejects output when optional recording was not enabled");
    test.check(missing_history_output.str().empty(),
               "missing history is rejected before a header is written");

    auto wrong_shape = result;
    wrong_shape.best_design.parameters.clear();
    std::ostringstream wrong_shape_output;
    test.check_throws<std::invalid_argument>(
        [&] {
            multobjopt::write_summary_json(wrong_shape_output, problem_definition, wrong_shape);
        },
        "summary writer rejects a result belonging to a different problem shape");
    test.check(wrong_shape_output.str().empty(),
               "shape mismatch is rejected before JSON output begins");

    std::ostringstream invalid_format_output;
    test.check_throws<std::invalid_argument>(
        [&] {
            multobjopt::write_pareto_front(invalid_format_output, problem_definition, result,
                                           static_cast<multobjopt::delimited_text_format>(99));
        },
        "tabular writer rejects an unknown format enumerator");
    test.check(invalid_format_output.str().empty(),
               "unknown format is rejected before tabular output begins");

    failing_stream_buffer buffer;
    std::ostream failed_output(&buffer);
    test.check_throws<std::ios_base::failure>(
        [&] { multobjopt::write_pareto_front(failed_output, problem_definition, result); },
        "a stream-buffer write failure is reported as ios_base::failure");

    return test.finish();
}
