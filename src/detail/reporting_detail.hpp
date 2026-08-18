#ifndef MULTOBJOPT_DETAIL_REPORTING_DETAIL_HPP
#define MULTOBJOPT_DETAIL_REPORTING_DETAIL_HPP

#include <multobjopt/reporting.hpp>

#include <cstddef>
#include <iosfwd>
#include <string>
#include <string_view>

/**
 * @file reporting_detail.hpp
 * @brief Shared private validation and encoding operations for report writers.
 */

namespace multobjopt::detail {

/** @brief Validate all result shapes needed by reporting before writing bytes. */
void validate_reporting_input(const problem& problem_definition, const optimization_result& result,
                              bool require_complete_history);

/** @brief Map a public delimited format to its separator character. */
[[nodiscard]] char delimiter_for(delimited_text_format format);

/** @brief Reject a stream that cannot currently accept report output. */
void require_writable_output(const std::ostream& output);

/** @brief Convert a scalar to stable locale-independent tabular text. */
[[nodiscard]] std::string format_report_scalar(scalar value);

/** @brief Convert an unsigned report index to locale-independent decimal text. */
[[nodiscard]] std::string format_report_index(std::size_t value);

/** @brief Write one escaped CSV/TSV field without a separator or line ending. */
void write_delimited_field(std::ostream& output, std::string_view value, char delimiter);

/** @brief Write the common named evaluated-design table header. */
void write_delimited_header(std::ostream& output, const problem& problem_definition,
                            std::string_view index_name, char delimiter);

/** @brief Write one indexed evaluated-design table row. */
void write_delimited_design(std::ostream& output, const evaluated_design& design,
                            std::size_t one_based_index, char delimiter);

/** @brief Write a JSON string with required escaping. */
void write_json_string(std::ostream& output, std::string_view value);

/** @brief Write one scalar as a JSON number or null when it is non-finite. */
void write_json_scalar(std::ostream& output, scalar value);

/** @brief Write the named best-design object used by the summary schema. */
void write_json_design(std::ostream& output, const problem& problem_definition,
                       const evaluated_design& design);

} // namespace multobjopt::detail

#endif
