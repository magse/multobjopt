#include "detail/reporting_detail.hpp"

#include <ostream>
#include <string>
#include <string_view>

/**
 * @file reporting_delimited.cpp
 * @brief Shared CSV/TSV field escaping, headers, and evaluated-design rows.
 */

namespace multobjopt::detail {
namespace {

void write_text(std::ostream& output, std::string_view text) {
    output.write(text.data(), static_cast<std::streamsize>(text.size()));
}

void write_separator(std::ostream& output, char delimiter) { output.put(delimiter); }

void write_named_header(std::ostream& output, std::string_view prefix, std::string_view name,
                        char delimiter) {
    std::string header(prefix);
    header.append(name);
    write_separator(output, delimiter);
    write_delimited_field(output, header, delimiter);
}

void write_scalar_field(std::ostream& output, scalar value, char delimiter) {
    write_separator(output, delimiter);
    const auto text = format_report_scalar(value);
    write_text(output, text);
}

} // namespace

/**
 * @brief Write one field using delimiter-aware double-quote escaping.
 *
 * This is the familiar CSV quoting convention applied equally to tab-separated
 * output: quote a field containing the active delimiter, a quote, CR, or LF,
 * and represent each embedded quote by two quotes. Names are never sanitized,
 * so the resulting header round-trips the exact problem definition text.
 */
void write_delimited_field(std::ostream& output, std::string_view value, char delimiter) {
    const bool needs_quotes = value.find(delimiter) != std::string_view::npos ||
                              value.find('"') != std::string_view::npos ||
                              value.find('\r') != std::string_view::npos ||
                              value.find('\n') != std::string_view::npos;
    if (!needs_quotes) {
        write_text(output, value);
        return;
    }

    output.put('"');
    for (const char character : value) {
        if (character == '"') {
            output.put('"');
        }
        output.put(character);
    }
    output.put('"');
}

/** @brief Write the fixed schema header shared by history and Pareto tables. */
void write_delimited_header(std::ostream& output, const problem& problem_definition,
                            std::string_view index_name, char delimiter) {
    write_delimited_field(output, index_name, delimiter);
    for (const auto& parameter : problem_definition.parameters()) {
        write_named_header(output, "parameter.", parameter.name, delimiter);
    }
    for (const auto& objective : problem_definition.objectives()) {
        write_named_header(output, "objective.", objective.name, delimiter);
    }
    for (const auto& restriction : problem_definition.restrictions()) {
        write_named_header(output, "restriction.", restriction.name, delimiter);
    }
    write_separator(output, delimiter);
    write_text(output, "valid");
    write_separator(output, delimiter);
    write_text(output, "feasible");
    write_separator(output, delimiter);
    write_text(output, "total_violation");
    write_separator(output, delimiter);
    write_text(output, "scalarized_objective\n");
}

/** @brief Write one complete evaluated-design record with a one-based index. */
void write_delimited_design(std::ostream& output, const evaluated_design& design,
                            std::size_t one_based_index, char delimiter) {
    const auto index_text = format_report_index(one_based_index);
    write_text(output, index_text);
    for (const scalar value : design.parameters) {
        write_scalar_field(output, value, delimiter);
    }
    for (const scalar value : design.objectives) {
        write_scalar_field(output, value, delimiter);
    }
    for (const scalar value : design.restrictions) {
        write_scalar_field(output, value, delimiter);
    }
    write_separator(output, delimiter);
    write_text(output, design.valid ? "true" : "false");
    write_separator(output, delimiter);
    write_text(output, design.feasible ? "true" : "false");
    write_scalar_field(output, design.total_violation, delimiter);
    write_scalar_field(output, design.scalarized_objective, delimiter);
    output.put('\n');
}

} // namespace multobjopt::detail
