#include "detail/reporting_detail.hpp"

#include <cmath>
#include <ostream>
#include <string_view>

/**
 * @file reporting_json.cpp
 * @brief Shared strict-JSON encoding for optimization summaries.
 */

namespace multobjopt::detail {
namespace {

void write_text(std::ostream& output, std::string_view text) {
    output.write(text.data(), static_cast<std::streamsize>(text.size()));
}

void write_json_member_prefix(std::ostream& output, std::string_view name) {
    write_text(output, "      ");
    write_json_string(output, name);
    write_text(output, ": ");
}

void write_parameter_values(std::ostream& output, const problem& problem_definition,
                            const evaluated_design& design) {
    write_text(output, "    \"parameters\": {");
    if (!problem_definition.parameters().empty()) {
        output.put('\n');
        for (std::size_t index = 0; index < problem_definition.parameters().size(); ++index) {
            write_json_member_prefix(output, problem_definition.parameters()[index].name);
            write_json_scalar(output, design.parameters[index]);
            write_text(output, index + 1 == problem_definition.parameters().size() ? "\n" : ",\n");
        }
        write_text(output, "    ");
    }
    write_text(output, "},\n");
}

void write_objective_values(std::ostream& output, const problem& problem_definition,
                            const evaluated_design& design) {
    write_text(output, "    \"objectives\": {");
    if (!problem_definition.objectives().empty()) {
        output.put('\n');
        for (std::size_t index = 0; index < problem_definition.objectives().size(); ++index) {
            write_json_member_prefix(output, problem_definition.objectives()[index].name);
            write_json_scalar(output, design.objectives[index]);
            write_text(output, index + 1 == problem_definition.objectives().size() ? "\n" : ",\n");
        }
        write_text(output, "    ");
    }
    write_text(output, "},\n");
}

void write_restriction_values(std::ostream& output, const problem& problem_definition,
                              const evaluated_design& design) {
    write_text(output, "    \"restrictions\": {");
    if (!problem_definition.restrictions().empty()) {
        output.put('\n');
        for (std::size_t index = 0; index < problem_definition.restrictions().size(); ++index) {
            write_json_member_prefix(output, problem_definition.restrictions()[index].name);
            write_json_scalar(output, design.restrictions[index]);
            write_text(output,
                       index + 1 == problem_definition.restrictions().size() ? "\n" : ",\n");
        }
        write_text(output, "    ");
    }
    write_text(output, "},\n");
}

} // namespace

/**
 * @brief Write one valid JSON string while preserving non-control UTF-8 bytes.
 *
 * Problem names are ordinary std::string values. Valid UTF-8 passes through
 * unchanged; quotation marks, backslashes, and every U+0000--U+001F byte are
 * escaped so they cannot change the JSON structure.
 */
void write_json_string(std::ostream& output, std::string_view value) {
    static constexpr char hex_digits[] = "0123456789abcdef";
    output.put('"');
    for (const unsigned char character : value) {
        switch (character) {
        case '"':
            write_text(output, "\\\"");
            break;
        case '\\':
            write_text(output, "\\\\");
            break;
        case '\b':
            write_text(output, "\\b");
            break;
        case '\f':
            write_text(output, "\\f");
            break;
        case '\n':
            write_text(output, "\\n");
            break;
        case '\r':
            write_text(output, "\\r");
            break;
        case '\t':
            write_text(output, "\\t");
            break;
        default:
            if (character < 0x20U) {
                write_text(output, "\\u00");
                output.put(hex_digits[(character >> 4U) & 0x0fU]);
                output.put(hex_digits[character & 0x0fU]);
            } else {
                output.put(static_cast<char>(character));
            }
            break;
        }
    }
    output.put('"');
}

/** @brief Emit a finite JSON number, using null for all non-finite scalars. */
void write_json_scalar(std::ostream& output, scalar value) {
    if (!std::isfinite(value)) {
        write_text(output, "null");
        return;
    }
    const auto text = format_report_scalar(value);
    write_text(output, text);
}

/** @brief Emit the named raw values and derived status of the selected design. */
void write_json_design(std::ostream& output, const problem& problem_definition,
                       const evaluated_design& design) {
    write_text(output, "{\n");
    write_parameter_values(output, problem_definition, design);
    write_objective_values(output, problem_definition, design);
    write_restriction_values(output, problem_definition, design);
    write_text(output, "    \"valid\": ");
    write_text(output, design.valid ? "true" : "false");
    write_text(output, ",\n    \"feasible\": ");
    write_text(output, design.feasible ? "true" : "false");
    write_text(output, ",\n    \"total_violation\": ");
    write_json_scalar(output, design.total_violation);
    write_text(output, ",\n    \"scalarized_objective\": ");
    write_json_scalar(output, design.scalarized_objective);
    write_text(output, "\n  }");
}

} // namespace multobjopt::detail
