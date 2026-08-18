#include "detail/reporting_detail.hpp"

#include <ostream>

/**
 * @file write_evaluation_history.cpp
 * @brief Public serialization of the optional complete evaluation trajectory.
 */

namespace multobjopt {

/**
 * @brief Write a complete captured evaluation history in chronological order.
 *
 * All preflight checks occur before the header is written. A later device or
 * stream-buffer failure can still leave partial output because a caller-owned
 * stream has no general rollback operation.
 */
void write_evaluation_history(std::ostream& output, const problem& problem_definition,
                              const optimization_result& result, delimited_text_format format) {
    const char delimiter = detail::delimiter_for(format);
    detail::validate_reporting_input(problem_definition, result, true);
    detail::require_writable_output(output);

    detail::write_delimited_header(output, problem_definition, "evaluation", delimiter);
    for (std::size_t index = 0; index < result.evaluation_history.size(); ++index) {
        detail::write_delimited_design(output, result.evaluation_history[index], index + 1,
                                       delimiter);
    }
    detail::require_writable_output(output);
}

} // namespace multobjopt
