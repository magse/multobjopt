#include "detail/reporting_detail.hpp"

#include <multobjopt/optimize.hpp>
#include <multobjopt/version.hpp>

#include <ostream>
#include <string_view>

/**
 * @file write_summary_json.cpp
 * @brief Public deterministic JSON summary serialization.
 */

namespace multobjopt {
namespace {

void write_text(std::ostream& output, std::string_view text) {
    output.write(text.data(), static_cast<std::streamsize>(text.size()));
}

void write_count(std::ostream& output, std::size_t value) {
    const auto text = detail::format_report_index(value);
    write_text(output, text);
}

} // namespace

/**
 * @brief Write schema version 1 of the compact optimization-result summary.
 *
 * The schema intentionally summarizes rather than embeds the two potentially
 * large tabular datasets. Their row counts let consumers associate separately
 * written history and Pareto files with this completed run.
 */
void write_summary_json(std::ostream& output, const problem& problem_definition,
                        const optimization_result& result) {
    detail::validate_reporting_input(problem_definition, result, false);
    detail::require_writable_output(output);

    write_text(output, "{\n  \"schema\": \"multobjopt.optimization_summary\",\n");
    write_text(output, "  \"schema_version\": 1,\n  \"library_version\": ");
    detail::write_json_string(output, version::library_version);
    write_text(output, ",\n  \"algorithm_used\": ");
    detail::write_json_string(output, to_string(result.algorithm_used));
    write_text(output, ",\n  \"termination_reason\": ");
    detail::write_json_string(output, to_string(result.reason));
    write_text(output, ",\n  \"evaluations\": ");
    write_count(output, result.evaluations);
    write_text(output, ",\n  \"iterations\": ");
    write_count(output, result.iterations);
    write_text(output, ",\n  \"evaluation_history_recorded\": ");
    write_text(output, result.evaluation_history.empty() ? "false" : "true");
    write_text(output, ",\n  \"evaluation_history_count\": ");
    write_count(output, result.evaluation_history.size());
    write_text(output, ",\n  \"pareto_front_count\": ");
    write_count(output, result.pareto_front.size());
    write_text(output, ",\n  \"best_design\": ");
    detail::write_json_design(output, problem_definition, result.best_design);
    write_text(output, "\n}\n");

    detail::require_writable_output(output);
}

} // namespace multobjopt
