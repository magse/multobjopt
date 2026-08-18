#include "detail/reporting_detail.hpp"

#include <ostream>

/**
 * @file write_pareto_front.cpp
 * @brief Public serialization of the final deterministic Pareto archive.
 */

namespace multobjopt {

/** @brief Write every final Pareto record using its existing archive order. */
void write_pareto_front(std::ostream& output, const problem& problem_definition,
                        const optimization_result& result, delimited_text_format format) {
    const char delimiter = detail::delimiter_for(format);
    detail::validate_reporting_input(problem_definition, result, false);
    detail::require_writable_output(output);

    detail::write_delimited_header(output, problem_definition, "pareto_index", delimiter);
    for (std::size_t index = 0; index < result.pareto_front.size(); ++index) {
        detail::write_delimited_design(output, result.pareto_front[index], index + 1, delimiter);
    }
    detail::require_writable_output(output);
}

} // namespace multobjopt
