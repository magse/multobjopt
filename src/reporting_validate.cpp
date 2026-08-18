#include "detail/reporting_detail.hpp"

#include <ios>
#include <ostream>
#include <stdexcept>
#include <string_view>

/**
 * @file reporting_validate.cpp
 * @brief Preflight validation for every public report writer.
 */

namespace multobjopt::detail {
namespace {

/**
 * Check the owning vector layout without rejecting legitimate non-finite model
 * results. Invalid evaluations are reportable data; only structural mismatch
 * means that the supplied problem cannot name the record unambiguously.
 */
void validate_design_shape(const problem& problem_definition, const evaluated_design& design,
                           std::string_view description) {
    if (design.parameters.size() != problem_definition.parameters().size()) {
        throw std::invalid_argument(std::string(description) +
                                    " parameter count does not match the problem");
    }
    if (design.objectives.size() != problem_definition.objectives().size()) {
        throw std::invalid_argument(std::string(description) +
                                    " objective count does not match the problem");
    }
    if (design.restrictions.size() != problem_definition.restrictions().size()) {
        throw std::invalid_argument(std::string(description) +
                                    " restriction count does not match the problem");
    }
}

} // namespace

/**
 * @brief Validate a problem/result pair completely before a report writes data.
 * @param problem_definition Definition supplying names and expected dimensions.
 * @param result Completed result whose owning records will be serialized.
 * @param require_complete_history Whether an evaluation-history table is being written.
 *
 * A non-empty optional history must always be complete. The stricter flag also
 * rejects an empty history when evaluations were performed, which catches the
 * common mistake of forgetting record_evaluation_history before optimizing.
 */
void validate_reporting_input(const problem& problem_definition, const optimization_result& result,
                              bool require_complete_history) {
    problem_definition.validate();
    validate_design_shape(problem_definition, result.best_design, "best design");

    for (const auto& design : result.pareto_front) {
        validate_design_shape(problem_definition, design, "Pareto design");
    }
    for (const auto& design : result.evaluation_history) {
        validate_design_shape(problem_definition, design, "history design");
    }

    const bool has_partial_history = !result.evaluation_history.empty() &&
                                     result.evaluation_history.size() != result.evaluations;
    const bool missing_required_history =
        require_complete_history && result.evaluation_history.size() != result.evaluations;
    if (has_partial_history || missing_required_history) {
        throw std::invalid_argument(
            "evaluation history is incomplete; enable record_evaluation_history before optimize");
    }
}

/**
 * @brief Return the separator represented by a supported public enumerator.
 * @throws std::invalid_argument for a cast or future value not understood here.
 */
char delimiter_for(delimited_text_format format) {
    switch (format) {
    case delimited_text_format::csv:
        return ',';
    case delimited_text_format::tsv:
        return '\t';
    }
    throw std::invalid_argument("unknown delimited text format");
}

/**
 * @brief Reject a bad destination before or after a serialization operation.
 *
 * Stream exception masks remain under caller control. Operators may therefore
 * throw ios_base::failure directly; this explicit check provides the same clear
 * failure signal when the caller left the usual non-throwing mask in place.
 */
void require_writable_output(const std::ostream& output) {
    if (!output) {
        throw std::ios_base::failure("failed to write multobjopt report");
    }
}

} // namespace multobjopt::detail
