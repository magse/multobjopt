#ifndef MULTOBJOPT_REPORTING_HPP
#define MULTOBJOPT_REPORTING_HPP

#include <multobjopt/problem.hpp>
#include <multobjopt/result.hpp>

#include <iosfwd>

/**
 * @file reporting.hpp
 * @brief Optional, deterministic serialization of completed optimization runs.
 *
 * Reporting is deliberately separate from optimize(): solving a problem never
 * opens, overwrites, or otherwise manages a file. Applications opt in by
 * calling one of these functions with a stream after optimization completes.
 * This keeps file names, ownership, buffering, and overwrite policy under the
 * application's control while supporting files, standard output, and in-memory
 * streams through the same API.
 */

namespace multobjopt {

/** @brief Supported delimiter conventions for tabular optimization reports. */
enum class delimited_text_format {
    csv, ///< Comma-separated fields.
    tsv  ///< Tab-separated fields.
};

/**
 * @brief Write every recorded design evaluation as CSV or TSV.
 *
 * The first column is the one-based `evaluation` number. It is followed by
 * normalized parameter values, raw objective values, raw restriction values,
 * and the derived validity, feasibility, violation, and scalarized score. The
 * definition names are preserved exactly in headers prefixed with
 * `parameter.`, `objective.`, or `restriction.`. Fields containing the selected
 * delimiter, a quote, or a line break use conventional double-quote escaping.
 *
 * History is an optional in-memory feature. Set
 * optimizer_options::record_evaluation_history before calling optimize(); this
 * writer rejects an incomplete history instead of silently producing a file
 * that appears to contain every evaluation.
 *
 * Numeric output is locale-independent and uses enough digits for scalar
 * round trips. Non-finite tabular values use `nan`, `inf`, and `-inf`. The
 * function writes a header even when a complete zero-evaluation result is
 * supplied. It does not invoke problem callbacks, flush, or close @p output.
 *
 * @param output Destination stream, which must initially be writable.
 * @param problem_definition Problem supplying ordered column names and counts.
 * @param result Completed optimization result with a complete recorded history.
 * @param format Comma- or tab-separated output convention.
 *
 * @throws std::invalid_argument If the problem is invalid, the format is not a
 *         recognized enumerator, a result vector has the wrong shape, or the
 *         recorded history is incomplete.
 * @throws std::ios_base::failure If the stream is not writable or a write fails.
 */
void write_evaluation_history(std::ostream& output, const problem& problem_definition,
                              const optimization_result& result,
                              delimited_text_format format = delimited_text_format::csv);

/**
 * @brief Write the completed feasible Pareto archive as CSV or TSV.
 *
 * The first column is the one-based `pareto_index` in the result's existing
 * deterministic archive order. Remaining columns and scalar formatting match
 * write_evaluation_history(). An empty Pareto front produces a header-only
 * table, which is a valid representation of a run with no feasible design.
 * The function does not invoke problem callbacks, flush, or close @p output.
 *
 * @param output Destination stream, which must initially be writable.
 * @param problem_definition Problem supplying ordered column names and counts.
 * @param result Completed optimization result whose Pareto front is written.
 * @param format Comma- or tab-separated output convention.
 *
 * @throws std::invalid_argument If the problem is invalid, the format is not a
 *         recognized enumerator, or a result vector has the wrong shape.
 * @throws std::ios_base::failure If the stream is not writable or a write fails.
 */
void write_pareto_front(std::ostream& output, const problem& problem_definition,
                        const optimization_result& result,
                        delimited_text_format format = delimited_text_format::csv);

/**
 * @brief Write a deterministic JSON summary of a completed optimization run.
 *
 * The document identifies its schema and library version, reports the actual
 * algorithm, termination reason, evaluation and iteration counts, recorded
 * history and Pareto counts, and names every value in the best design. It does
 * not duplicate the potentially large history or Pareto tables. JSON has no
 * representation for NaN or infinity, so every non-finite scalar is emitted as
 * `null`; evaluated_design::valid retains the associated validity information.
 *
 * Output uses declaration order, two-space indentation, and a final newline.
 * No timestamp or other nondeterministic process data is added. The function
 * does not invoke problem callbacks, flush, or close @p output.
 *
 * @param output Destination stream, which must initially be writable.
 * @param problem_definition Problem supplying ordered value names and counts.
 * @param result Completed optimization result to summarize.
 *
 * @throws std::invalid_argument If the problem is invalid or a result vector
 *         has the wrong shape.
 * @throws std::ios_base::failure If the stream is not writable or a write fails.
 */
void write_summary_json(std::ostream& output, const problem& problem_definition,
                        const optimization_result& result);

} // namespace multobjopt

#endif
