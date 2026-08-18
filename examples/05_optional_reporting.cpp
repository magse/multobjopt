/**
 * @file 05_optional_reporting.cpp
 * @brief Opt-in CSV/TSV trajectory, Pareto-front, and JSON summary reports.
 *
 * Run without arguments to perform the optimization without creating files.
 * Pass an output prefix and optional `csv` or `tsv` table format to write three
 * reports after the solver has finished, for example:
 *
 * @code{.sh}
 * ./multobjopt_example_05_optional_reporting actuator_run tsv
 * @endcode
 */

#include <multobjopt/multobjopt.hpp>

#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

/**
 * @brief Open, write, and explicitly close one report file.
 *
 * Enabling stream exceptions makes path, capacity, and close-time failures
 * visible to the application instead of silently leaving a partial report.
 */
template <typename writer_type> void write_file(const std::string& path, writer_type&& writer) {
    std::ofstream output;
    output.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    output.open(path, std::ios_base::binary | std::ios_base::trunc);
    std::forward<writer_type>(writer)(output);
    output.close();
}

} // namespace

int main(int argument_count, char* arguments[]) {
    using namespace multobjopt;

    if (argument_count > 3) {
        std::cerr << "usage: " << arguments[0] << " [output_prefix [csv|tsv]]\n";
        return EXIT_FAILURE;
    }

    delimited_text_format table_format = delimited_text_format::csv;
    std::string table_extension = "csv";
    if (argument_count == 3) {
        const std::string_view requested_format{arguments[2]};
        if (requested_format == "tsv") {
            table_format = delimited_text_format::tsv;
            table_extension = "tsv";
        } else if (requested_format != "csv") {
            std::cerr << "table format must be csv or tsv\n";
            return EXIT_FAILURE;
        }
    }

    // This one-dimensional trade-off produces a useful Pareto table while
    // keeping the report columns easy to inspect by hand.
    problem problem_definition;
    problem_definition.add_parameter("position", 0.0, 1.0, 0.05)
        .add_objective("distance_from_left", objective_sense::minimize,
                       [](scalar_view parameters) { return parameters[0] * parameters[0]; })
        .add_objective("distance_from_right", objective_sense::minimize,
                       [](scalar_view parameters) {
                           const scalar distance = parameters[0] - 1.0;
                           return distance * distance;
                       });

    optimizer_options options;
    options.algorithm = optimization_algorithm::genetic_algorithm;
    options.random_seed = 20'260'818;
    options.max_evaluations = 500;
    options.max_iterations = 100;
    options.stall_iterations = 40;
    options.population_size = 32;

    // History capture is independent of report writing. It is disabled by
    // default because retaining every evaluation may consume significant
    // memory in a long run. Pareto and summary reports need no history.
    options.record_evaluation_history = true;

    const optimization_result result = optimize(problem_definition, options);
    std::cout << "evaluations: " << result.evaluations << '\n'
              << "recorded evaluations: " << result.evaluation_history.size() << '\n'
              << "pareto designs: " << result.pareto_front.size() << '\n';

    // With no prefix, no reporting function is called and no file is created.
    // This is also how the example runs under CTest.
    if (argument_count == 1) {
        std::cout << "no report files requested; pass an output prefix to create them\n";
        return result.best_design.feasible ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    const std::string prefix{arguments[1]};
    const std::string history_path = prefix + "_evaluations." + table_extension;
    const std::string pareto_path = prefix + "_pareto." + table_extension;
    const std::string summary_path = prefix + "_summary.json";

    // Writers accept ordinary std::ostream objects. Applications therefore
    // control filenames, overwriting policy, buffering, compression adapters,
    // sockets, or in-memory std::ostringstream destinations themselves.
    write_file(history_path, [&](std::ostream& output) {
        write_evaluation_history(output, problem_definition, result, table_format);
    });
    write_file(pareto_path, [&](std::ostream& output) {
        write_pareto_front(output, problem_definition, result, table_format);
    });
    write_file(summary_path, [&](std::ostream& output) {
        write_summary_json(output, problem_definition, result);
    });

    std::cout << "wrote " << history_path << '\n'
              << "wrote " << pareto_path << '\n'
              << "wrote " << summary_path << '\n';
    return result.best_design.feasible ? EXIT_SUCCESS : EXIT_FAILURE;
}
